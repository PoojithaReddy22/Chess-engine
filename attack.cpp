#include"move.hpp"

//Allowed moves for knight, bishop, rook, king
static const int NMov[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
static const int BMov[4] = {-9, -11, 11, 9};
static const int RMov[4] = {-1, -10, 1, 10};
static const int KMov[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

bool IsSqAttacked(const int sq, const int side, std::shared_ptr<const BOARD> pos) {

#ifdef DEBUG
  assert(OnBoard(sq));
  assert(SideValid(side));
  assert(CheckBoard(pos));
#endif


//  Is square attacked by pawn
  if (side == WHITE) {
    if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
      return true;
    }
  } else {
    if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
      return true;
    }
  }

//  Is square attacked by knight
  for (int move : NMov) {
    int pce = pos->pieces[sq + move];
    if (isN(pce) && pieceCol[pce] == side)
      return true;
  }

//  Is square attacked by bishop or queen
  for (int move : BMov) {
    int t_sq = sq + move;
    int pce = pos->pieces[t_sq];
//    Sliding pieces move till the end of the board
    while (pce != OFF_BOARD) {
      if (pce != EMPTY) {
        if ((isB(pce) || isQ(pce)) && pieceCol[pce] == side)
          return true;
        break;
      }
      t_sq += move;
      pce = pos->pieces[t_sq];
    }
  }

//  Is square attacked by rook or queen
  for (int move : RMov) {
    int t_sq = sq + move;
    int pce = pos->pieces[t_sq];
//    Sliding pieces move till the end of the board
    while (pce != OFF_BOARD) {
      if (pce != EMPTY) {
        if ((isR(pce) || isQ(pce)) && pieceCol[pce] == side)
          return true;
        break;
      }
      t_sq += move;
      pce = pos->pieces[t_sq];
    }
  }

//  Is square attacked by king
  for (int move : KMov) {
    int pce = pos->pieces[sq + move];
    if (isK(pce) && pieceCol[pce] == side)
      return true;
  }

  return false;
}