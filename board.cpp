#include"board.hpp"
#include"hash.hpp"
#include<chrono>

int Sq120ToSq64[BRDSQ_120];
int Sq64ToSq120[64];

int FilesBrd[BRDSQ_120];
int RanksBrd[BRDSQ_120];

std::string PiceChar = ".PNBRQKpnbrqk";
std::string SideChar = "wb-";

int pieceBig[13] = { false, false, true, true, true, true, true, false, true, true, true, true, true };
int pieceMaj[13] = { false, false, false, false, true, true, true, false, false, false, true, true, true };
int pieceMin[13] = { false, false, true, true, false, false, false, false, true, true, false, false, false };
int pieceVal[13] = { 0, 100, 320, 330, 500, 900, 20000, 100, 320, 330, 500, 900, 20000 };
int pieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };
int piecePwn[13] = { false, true, false, false, false, false, false, true, false, false, false, false, false };

BOARD::BOARD() {
  PvTable = std::make_unique<PVTABLE>(PVTABLE(PvSize));
  PvArray = std::unique_ptr<int[]>(new int[MAXDEPTH]);
  InitBoard();
  InitHash();
}

void InitBoard() {

//  Initialize with out of board values
  std::fill(Sq120ToSq64, Sq120ToSq64 + BRDSQ_120, 65);
  std::fill(Sq64ToSq120, Sq64ToSq120 + 64, 120);

//  Initialize the files and ranks array with off board values
  std::fill(FilesBrd, FilesBrd + BRDSQ_120, OFF_BOARD);
  std::fill(RanksBrd, RanksBrd + BRDSQ_120, OFF_BOARD);

  int sq64 = 0;
  for(int rank = RANK_1; rank <= RANK_8; ++rank) {
    for(int file = FILE_A; file <= FILE_H; ++file) {
      int sq = FR2SQ(file, rank);
      Sq120ToSq64[sq] = sq64;
      Sq64ToSq120[sq64++] = sq;
      FilesBrd[sq] = file;
      RanksBrd[sq] = rank;
    }
  }
}

void ResetBoard(std::shared_ptr<BOARD> pos) {

//  Set all the pieces as off board
  std::fill(pos->pieces, pos->pieces + BRDSQ_120, OFF_BOARD);

//  Set the pieces in the 8x8 square as empty
  for(int idx = 0; idx < 64; ++idx)
    pos->pieces[SQ120(idx)] = EMPTY;

//  Empty the arrays
  std::fill(pos->bigPce, pos->bigPce + 2, 0);
  std::fill(pos->majPce, pos->majPce + 2, 0);
  std::fill(pos->minPce, pos->minPce + 2, 0);
  std::fill(pos->material, pos->material + 2, 0);
  
  std::fill(pos->pceNum, pos->pceNum + 13, 0);

  pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;
  pos->side = BOTH;
  pos->enPas = NO_SQ;
  pos->castlePerm = 0;
  pos->ply = 0;
  pos->hisPly = 0;
  pos->fifty_move = 0;
  pos->key = 0;
}

void PrintBoard() {
  for (int i = 0; i < BRDSQ_120; ++i) {
    if (i % 10 == 0) 
      std::cout << '\n';
    std::cout << std::setw(5) << Sq120ToSq64[i];
  }

  std::cout << '\n' << '\n';

  for(int i = 0; i < 64; ++i){
    if(i % 8 == 0)
      std::cout << '\n';
    std::cout << std::setw(5) << Sq64ToSq120[i];
  }

  std::cout << '\n' << '\n';
}

void PrintBoard(std::shared_ptr<BOARD> pos) {

  std::cout << "BOARD\n\n";

//  Print the board pieces
  for(int rank = RANK_8; rank >= RANK_1; --rank){
    std::cout << std::setw(5) << rank + 1;
    for(int file = FILE_A; file <= FILE_H; ++file){
      int sq120 = FR2SQ(file, rank);
      int piece = pos->pieces[sq120];
      std::cout << std::setw(5) << PiceChar[piece];
    }
    std::cout << '\n';
  }

  std::cout << '\n';
  std::cout << std::setw(5) << ' ';
  for(int file = FILE_A; file <= FILE_H; ++file)
    std::cout << std::setw(5) << char(file + 'a');

  std::cout << '\n';

//  Print side and en passant square
  std::cout << std::setw(10) << "side : " << SideChar[pos->side] << '\n';
  std::cout << std::setw(10) << "enPas : " << char(FilesBrd[pos->enPas] + 'a') << RanksBrd[pos->enPas] + 1 <<'\n';

//  Print available castling permissions
  std::cout << std::setw(10) << "castle : ";
  std::cout << (pos->castlePerm & WKCA ? 'K' : '-');
  std::cout << (pos->castlePerm & WQCA ? 'Q' : '-');
  std::cout << (pos->castlePerm & BKCA ? 'k' : '-');
  std::cout << (pos->castlePerm & BQCA ? 'q' : '-');

  std::cout << '\n';

//  Print the 64 bit position key
  std::cout << std::setw(10) << "posKey : " << pos->key;

  std::cout << "\n\n";
}

void UpdateListMaterial(std::shared_ptr<BOARD> pos) {
  for(int idx = 0; idx < BRDSQ_120; ++idx) {
    int piece = pos->pieces[idx];
    if(piece != OFF_BOARD && piece != EMPTY) {
      int color = pieceCol[piece];

      if(pieceBig[piece]) ++pos->bigPce[color];
      if(pieceMaj[piece]) ++pos->majPce[color];
      if(pieceMin[piece]) ++pos->minPce[color];

      pos->material[color] += pieceVal[piece];

      pos->pList[piece][pos->pceNum[piece]] = idx;
      ++pos->pceNum[piece];

      if(piece == wK || piece == bK) pos->KingSq[color] = idx;
    }
  }
}

bool CheckBoard(std::shared_ptr<const BOARD> pos) {
  int t_pceNum[13] = {};
  int t_bigPce[2] = {};
  int t_majPce[2] = {};
  int t_minPce[2] = {};
  int t_material[2] = {};

//  Check squares in the piece list map to the corresponding pieces in the position of pieces
  for(int t_piece = wP; t_piece <= bK; ++t_piece){
    for(int t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; ++t_pce_num){
      int sq120 = pos->pList[t_piece][t_pce_num];
//      std::cout << sq120 << ' ' << t_piece << ' ' << pos->pieces[sq120] << '\n';
      assert(pos->pieces[sq120] == t_piece);
    }
  }

//  Check material, bigPce, majPce and minPce are correctly set
  for(int sq64 = 0; sq64 < 64; ++sq64){
    int sq120 = SQ120(sq64);
    int t_pce = pos->pieces[sq120];
    int color = pieceCol[t_pce];
    
    ++t_pceNum[t_pce];

    if(color != BOTH) {
      if(pieceBig[t_pce]) ++t_bigPce[color];
      if(pieceMaj[t_pce]) ++t_majPce[color];
      if(pieceMin[t_pce]) ++t_minPce[color];

      t_material[color] += pieceVal[t_pce];
    }
  }

  assert(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
  assert(t_bigPce[WHITE] == pos->bigPce[WHITE] && t_bigPce[BLACK] == pos->bigPce[BLACK]);
  assert(t_majPce[WHITE] == pos->majPce[WHITE] && t_majPce[BLACK] == pos->majPce[BLACK]);
  assert(t_minPce[WHITE] == pos->minPce[WHITE] && t_minPce[BLACK] == pos->minPce[BLACK]);

//  assert side is valid
  assert(pos->side == WHITE || pos->side == BLACK);

//  Key corresponding to a state should be the same
  if(GeneratePosKey(pos) != pos->key)
    getchar();
  
  assert(GeneratePosKey(pos) == pos->key);

//  En-passant possible only in the 3rd and the 6th rank
  assert(pos->enPas == NO_SQ 
          || (RanksBrd[pos->enPas] == RANK_6 && pos->side == WHITE)
          || (RanksBrd[pos->enPas] == RANK_3 && pos->side == BLACK));

//  Check if the position of the kings are correctly stored
  if(pos->pieces[pos->KingSq[WHITE]] != wK)
    getchar();

  assert(pos->pieces[pos->KingSq[WHITE]] == wK);
  assert(pos->pieces[pos->KingSq[BLACK]] == bK);

  return true;
}

unsigned U64 GetTime() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}
