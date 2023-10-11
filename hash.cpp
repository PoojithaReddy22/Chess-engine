#include"hash.hpp"

unsigned U64 PieceKeys[13][120];
unsigned U64 SideKey;
unsigned U64 CastleKeys[16];

void InitHash() {
  for(int piece = EMPTY; piece <= bK; ++piece)
    for(int pos = 0; pos < BRDSQ_120; ++pos)
      PieceKeys[piece][pos] = RAND_64;

  SideKey = RAND_64;

  for(int key = 0; key < 16; ++key)
    CastleKeys[key] = RAND_64;  
}

unsigned U64 GeneratePosKey(std::shared_ptr<const BOARD> pos){
  unsigned U64 FinalKey = 0;

//  XOR the pieces
  for(int sq = 0; sq < BRDSQ_120; ++sq) {
    int piece = pos->pieces[sq];
    if(piece != EMPTY && piece != OFF_BOARD && piece != NO_SQ) {
      assert(piece >= wP && piece <= bK);
      FinalKey ^= PieceKeys[piece][sq];
    }
  }

//  XOR the side
  if(pos->side == WHITE)
    FinalKey ^= SideKey;

//  XOR en passant square
  if(pos->enPas != NO_SQ) {
    assert(pos->enPas >= 0 && pos->enPas < BRDSQ_120);
    FinalKey ^= PieceKeys[EMPTY][pos->enPas];
  }

//  XOR castling keys
  assert(pos->castlePerm >= 0 && pos->castlePerm <= 15);
  FinalKey ^= CastleKeys[pos->castlePerm];

  return FinalKey; 
}