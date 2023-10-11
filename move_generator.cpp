#include"move.hpp"
#include <algorithm>

//Allowed non attack moves for white and black pawns
static const int PMov[2][2] = {{10, 20}, {-10, -20}};
//Allowed attack moves for white and black pawns
static const int PAttack[2][2] = {{9, 11}, {-9, -11}};

static const int SlidePieces[2][3] = {{wB, wR, wQ}, {bB, bR, bQ}};
static const int NonSlidePce[2][2] = {{wN, wK}, {bN, bK}};

static const int VictimScore[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};
static int MvvLvaScores[13][13];

static const int Mov[13][8] = {
  {},
  {},
  {-8, -19, -21, -12, 8, 19, 21, 12},
  {-9, -11, 11, 9},
  {-1, -10, 1, 10},
  {-1, -10, 1, 10, -9, -11, 9, 11},
  {-1, -10, 1, 10, -9, -11, 11, 9},
  {},
  {-8, -19, -21, -12, 8, 19, 21, 12},
  {-9, -11, 11, 9},
  {-1, -10, 1, 10},
  {-1, -10, 1, 10, -9, -11, 9, 11},
  {-1, -10, 1, 10, -9, -11, 11, 9}
};

static void InitMvvLva() {
  for(int attacker = wP; attacker <= bK; ++attacker) {
    for(int victim = wP; victim <= bK; ++victim) {
      MvvLvaScores[victim][attacker] = VictimScore[victim] + 6 - VictimScore[attacker] / 100;
    }
  }
}

bool MOVE::operator>(const MOVE &m) const {
  return score > m.score;
}

MOVE_LIST::MOVE_LIST() {
  InitMvvLva();
}

void MOVE_LIST::SortMoves() {
  std::sort(moves, moves + count, std::greater<MOVE>());
}

void PrintMoveList(std::shared_ptr<const MOVE_LIST> list) {
  for(int idx = 0; idx < list->count; ++idx)
    std::cout << "Move: " << idx << " > " << PrMove(list->moves[idx].move) 
              << ' ' << "(score:" << list->moves[idx].score << ')' << '\n';
}

static void AddQuietMove(int move, std::shared_ptr<MOVE_LIST> list, std::shared_ptr<const BOARD> pos) {
  list->moves[list->count].move = move;
  if(pos->searchKillers[0][pos->ply] == move) {
    list->moves[list->count].score = 9e5;
  } else if(pos->searchKillers[1][pos->ply] == move) {
    list->moves[list->count].score = 8e5;
  } else {
    list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
  }
  ++list->count;
}

static void AddCaptureMove(int move, std::shared_ptr<MOVE_LIST> list, std::shared_ptr<const BOARD> pos) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1e6;
  ++list->count;
}

static void AddPawnMove(std::shared_ptr<const BOARD> pos, const int from, const int to, const int cap, const int side,
                        std::shared_ptr<MOVE_LIST> list) {
  assert(SideValid(side));
  assert(OnBoard(from));
  assert(OnBoard(to));
  assert(PceValidEmpty(cap));

  if(side == WHITE) {
    if(RanksBrd[from] == RANK_7) {
//      Promotion moves
      for(int pro = wQ; pro >= wN; --pro)
        AddCaptureMove(MOVE(from, to, cap, pro, 0), list, pos);
    } else
      AddCaptureMove(MOVE(from, to, cap, EMPTY, 0), list, pos);
  } else {
    if(RanksBrd[from] == RANK_2) {
//      Promotion moves
      for(int pro = bQ; pro >= bN; --pro)
        AddCaptureMove(MOVE(from, to, cap, pro, 0), list, pos);
    } else
      AddCaptureMove(MOVE(from, to, cap, EMPTY, 0), list, pos);
  }
}

static void AddPawnMove(std::shared_ptr<const BOARD> pos, const int from, const int to, const int side,
                        std::shared_ptr<MOVE_LIST> list) {
  assert(SideValid(side));
  assert(OnBoard(from));
  assert(OnBoard(to));

  if(side == WHITE) {
    if(RanksBrd[from] == RANK_7) {
//      Promotion moves
      for(int pro = wQ; pro >= wN; --pro)
        AddQuietMove(MOVE(from, to, EMPTY, pro, EMPTY), list, pos);
    } else
      AddQuietMove(MOVE(from, to, EMPTY, EMPTY, 0), list, pos);
  } else {
    if(RanksBrd[from] == RANK_2) {
//      Promotion moves
      for(int pro = bQ; pro >= bN; --pro)
        AddQuietMove(MOVE(from, to, EMPTY, pro, EMPTY), list, pos);
    } else
      AddQuietMove(MOVE(from, to, EMPTY, EMPTY, 0), list, pos);
  }
}
void GenerateAllCaptureMoves(std::shared_ptr<const BOARD> pos, std::shared_ptr<MOVE_LIST> list) {

#ifdef DEBUG
  assert(CheckBoard(pos));
#endif

  int side = pos->side;

  assert(SideValid(side));

  if(side == WHITE) {

//    Loop through all the pawns on the board
    for(int pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
      int sq = pos->pList[wP][pceNum];
      assert(OnBoard(sq));

//      Check captures by pawns
      for(int idx = 0; idx < 2; ++idx) {
        if(!SQOFFBOARD(sq + PAttack[WHITE][idx]) && pieceCol[pos->pieces[sq + PAttack[WHITE][idx]]] == BLACK)
          AddPawnMove(pos, sq, sq + PAttack[WHITE][idx], pos->pieces[sq + PAttack[WHITE][idx]],WHITE, list);
        if(pos->enPas != NO_SQ && sq + PAttack[WHITE][idx] == pos->enPas)
          AddCaptureMove(MOVE(sq, pos->enPas, EMPTY, EMPTY, EPFLAG), list, pos);
      }
    }
  } else {

//    Loop through all the pawns on the board
    for (int pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
      int sq = pos->pList[bP][pceNum];
      assert(OnBoard(sq));

//      Check captures by pawns
      for (int idx = 0; idx < 2; ++idx) {
        if (!SQOFFBOARD(sq + PAttack[BLACK][idx]) && pieceCol[pos->pieces[sq + PAttack[BLACK][idx]]] == WHITE)
          AddPawnMove(pos, sq, sq + PAttack[BLACK][idx],
                      pos->pieces[sq + PAttack[BLACK][idx]], BLACK, list);
        if (pos->enPas != NO_SQ && sq + PAttack[BLACK][idx] == pos->enPas)
          AddCaptureMove(MOVE(sq, pos->enPas, EMPTY, EMPTY, EPFLAG), list, pos);
      }
    }
  }

//  Loop through non sliders aka knight and king
  for(int idx = 0; idx < 2; ++idx) {
    int pce = NonSlidePce[side][idx];
    for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
      int sq = pos->pList[pce][pceNum];
      assert(OnBoard(sq));
      for(int inc = 0; inc < 8; ++inc) {
        int to = sq + Mov[pce][inc];
        if(SQOFFBOARD(to))
          continue;
        if(pos->pieces[to] != EMPTY && pieceCol[pos->pieces[to]] == side ^ 1)
          AddCaptureMove(MOVE(sq, to, pos->pieces[to], EMPTY, EMPTY), list, pos);
      }
    }
  }

//  Loop through sliders aka queen, rook, bishop
  for(int idx = 0; idx < 3; ++idx) {
    int pce = SlidePieces[side][idx];
    for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
      int sq = pos->pList[pce][pceNum];
      assert(OnBoard(sq));
      int limit = 4;
      if(isQ(pce))
        limit = 8;
      for(int inc = 0; inc < limit; ++inc) {
        int dir = Mov[pce][inc];
        int to = sq + dir;
        while(!SQOFFBOARD(to)) {
          if(pos->pieces[to] != EMPTY) {
            if(pieceCol[pos->pieces[to]] == side ^ 1)
              AddCaptureMove(MOVE(sq, to, pos->pieces[to], EMPTY, EMPTY), list, pos);
            break;
          }
          to += dir;
        }
      }
    }
  }
}

void GenerateAllQuietMoves(std::shared_ptr<const BOARD> pos, std::shared_ptr<MOVE_LIST> list) {

#ifdef DEBUG
  assert(CheckBoard(pos));
#endif

  int side = pos->side;

  assert(SideValid(side));

  if(side == WHITE) {

//    Loop through all the pawns on the board
    for(int pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
      int sq = pos->pList[wP][pceNum];
      assert(OnBoard(sq));

//      1 step pawn move
      if(pos->pieces[sq + PMov[WHITE][0]] == EMPTY) {
        AddPawnMove(pos, sq, sq + PMov[WHITE][0], WHITE, list);
//        2 step pawn move
        if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + PMov[WHITE][1]] == EMPTY)
          AddQuietMove(MOVE(sq, sq + PMov[WHITE][1], EMPTY, EMPTY, PSFLAG), list, pos);
      }
    }

//    King side castle
    if(pos->castlePerm & WKCA) {
      if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
        if(!IsSqAttacked(E1, BLACK, pos) &&
           !IsSqAttacked(F1, BLACK, pos) && !IsSqAttacked(G1, BLACK, pos))
          AddQuietMove(MOVE(E1, G1, EMPTY, EMPTY, CSFLAG), list, pos);
      }
    }

//    Queen side castle
    if(pos->castlePerm & WQCA) {
      if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY)
        if(!IsSqAttacked(E1, BLACK, pos) && !IsSqAttacked(D1, BLACK, pos))
          AddQuietMove(MOVE(E1, C1, EMPTY, EMPTY, CSFLAG), list, pos);
    }

  } else {

//    Loop through all the pawns on the board
    for(int pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
      int sq = pos->pList[bP][pceNum];
      assert(OnBoard(sq));

//      1 step pawn move
      if(pos->pieces[sq + PMov[BLACK][0]] == EMPTY) {
        AddPawnMove(pos, sq, sq + PMov[BLACK][0], BLACK, list);
//        2 step pawn move
        if(RanksBrd[sq] == RANK_7 && pos->pieces[sq + PMov[BLACK][1]] == EMPTY)
          AddQuietMove(MOVE(sq, sq + PMov[BLACK][1], EMPTY, EMPTY, PSFLAG), list, pos);
      }
    }

//    King side castle
    if(pos->castlePerm & BKCA) {
      if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
        if(!IsSqAttacked(E8, WHITE, pos) && !IsSqAttacked(F8, WHITE, pos))
          AddQuietMove(MOVE(E8, G8, EMPTY, EMPTY, CSFLAG), list, pos);
      }
    }

//    Queen side castle
    if(pos->castlePerm & BQCA) {
      if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY)
        if(!IsSqAttacked(E8, WHITE, pos) && !IsSqAttacked(D8, WHITE, pos))
          AddQuietMove(MOVE(E8, C8, EMPTY, EMPTY, CSFLAG), list, pos);
    }
  }

//  Loop through non sliders aka knight and king
  for(int idx = 0; idx < 2; ++idx) {
    int pce = NonSlidePce[side][idx];
    for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
      int sq = pos->pList[pce][pceNum];
      assert(OnBoard(sq));
      for(int inc = 0; inc < 8; ++inc) {
        int to = sq + Mov[pce][inc];
        if(SQOFFBOARD(to))
          continue;
        if(pos->pieces[to] == EMPTY)
          AddQuietMove(MOVE(sq, to, EMPTY, EMPTY, EMPTY), list, pos);
      }
    }
  }

//  Loop through sliders aka queen, rook, bishop
  for(int idx = 0; idx < 3; ++idx) {
    int pce = SlidePieces[side][idx];
    for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
      int sq = pos->pList[pce][pceNum];
      assert(OnBoard(sq));
      int limit = 4;
      if(isQ(pce))
        limit = 8;
      for(int inc = 0; inc < limit; ++inc) {
        int dir = Mov[pce][inc];
        int to = sq + dir;
        while(!SQOFFBOARD(to)) {
          if(pos->pieces[to] == EMPTY) {
            AddQuietMove(MOVE(sq, to, EMPTY, EMPTY, EMPTY), list, pos);
          } else
            break;
          to += dir;
        }
      }
    }
  }
}

void GenerateAllMoves(std::shared_ptr<const BOARD> pos, std::shared_ptr<MOVE_LIST> list) {
  list->count = 0;
  GenerateAllQuietMoves(pos, list);
  GenerateAllCaptureMoves(pos, list);
}