#include"move.hpp"
#include"hash.hpp"

static void ClearPiece(const int sq, std::shared_ptr<BOARD> pos) {
  assert(OnBoard(sq));

  int pce = pos->pieces[sq];

  assert(PceValid(pce));

  int color = pieceCol[pce];
  int t_pceNum = -1;

//  Clear piece from key
  pos->key ^= PieceKeys[pce][sq];

//  Clear piece from square and material
  pos->pieces[sq] = EMPTY;
  pos->material[color] -= pieceVal[pce];

//  Reduce count for big, maj, min pieces
  if(pieceBig[pce]) {
    --pos->bigPce[color];
    if(pieceMaj[pce])
      --pos->majPce[color];
    else
      --pos->minPce[color];
  }

  for(int idx = 0; idx < pos->pceNum[pce]; ++idx) {
    if(pos->pList[pce][idx] == sq) {
      t_pceNum = idx;
      break;
    }
  }

  assert(t_pceNum != -1);

//  Swap from the last term and delete the last term
  pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce] - 1];
  --pos->pceNum[pce];
//  std::cout << "Piece cleared at: " << sq << '\n';
}

static void AddPiece(const int sq, std::shared_ptr<BOARD> pos, const int pce) {
  assert(OnBoard(sq));
  assert(PceValid(pce));

  int color = pieceCol[pce];

//  Add piece to the key
  pos->key ^= PieceKeys[pce][sq];

//  Add piece to the square and material
  pos->pieces[sq] = pce;
  pos->material[color] += pieceVal[pce];

//  Increment corresponding big, maj, min pieces
  if(pieceBig[pce]) {
    ++pos->bigPce[color];
    if(pieceMaj[pce])
      ++pos->majPce[color];
    else
      ++pos->minPce[color];
  }

//  Increment piece num and add the piece to the back of plist
  ++pos->pceNum[pce];
  pos->pList[pce][pos->pceNum[pce] - 1] = sq;
}

static void MovePiece(const int from, const int to, std::shared_ptr<BOARD> pos) {
  assert(from != OFF_BOARD && to != OFF_BOARD);

  int pce = pos->pieces[from];

  if(!PceValid(pce))
    getchar();

//  std::cout << PrSq(from) << ' ' << PrSq(to) << ' ' << pce << std::endl;
  assert(PceValid(pce));

//  Remove the piece from the from square
  pos->key ^= PieceKeys[pce][from];
  pos->pieces[from] = EMPTY;

//  Add the piece to the to square
  pos->key ^= PieceKeys[pce][to];
  pos->pieces[to] = pce;

  int t_idx = -1;
//  Change the index of the piece from the from square to the to square
  for(int idx = 0; idx < pos->pceNum[pce]; ++idx) {
    if(pos->pList[pce][idx] == from) {
      pos->pList[pce][idx] = to;
      t_idx = pos->pList[pce][idx];
      break;
    }
  }

  assert(t_idx != -1);
}

static int CastlePerm(int sq) {
  switch(sq) {
    case E1 : return 12;
    case E8 : return 3;
    case H1 : return 14;
    case H8 : return 11;
    case A1 : return 13;
    case A8 : return 7;
    default : return 15; 
  }
}

bool MakeMove(std::shared_ptr<BOARD> pos, int move) {
//std::cout << PrMove(move) << std::endl;
//  std::cout << "Make move" << std::endl;
#ifdef DEBUG
  assert(CheckBoard(pos));
#endif

  int from = FROMSQ(move);
  int to = TOSQ(move);
  int side = pos->side;

  assert(from != OFF_BOARD);
  assert(to != OFF_BOARD);
  assert(SideValid(side));
  assert(OnBoard(from));

//  Store current move in history array
  pos->history[pos->hisPly].key = pos->key;
  pos->history[pos->hisPly].move = move;
  pos->history[pos->hisPly].fiftymove = pos->fifty_move;
  pos->history[pos->hisPly].enPas = pos->enPas;
  pos->history[pos->hisPly].castlePerm = pos->castlePerm;

//  If en passant flag is set then clear the attacked pawn
  if(move & EPFLAG) {
    if(side == WHITE)
      ClearPiece(to - 10, pos);
    else
      ClearPiece(to + 10, pos);
  } else if(move & CSFLAG) {
//    If castling move than move the corresponding rooks
    switch(to) {
      case C1 :
        MovePiece(A1, D1, pos);  break;
      case C8 :
        MovePiece(A8, D8, pos);  break;
      case G1 :
        MovePiece(H1, F1, pos);  break;
      case G8 :
        MovePiece(H8, F8, pos);  break;
      default :  assert(false);
    } 
  }

//  If en passant square is set, than unset it
  if(pos->enPas != NO_SQ) pos->key ^= PieceKeys[EMPTY][pos->enPas];
//  Unset the castling permission from the key
  pos->key ^= CastleKeys[pos->castlePerm];

//  unset the castling permission if the move is from or to one of the special squares
  pos->castlePerm &= CastlePerm(from);
  pos->castlePerm &= CastlePerm(to);
  pos->enPas = NO_SQ;

//  Set the castling permission to the key
  pos->key ^= CastleKeys[pos->castlePerm];

//  Get the captured piece
  int captured = CAPTURED(move);
  ++pos->fifty_move;

  if(captured != EMPTY) {
    assert(PceValid(captured));
//    Clear the piece on the to square if its captured
    ClearPiece(to, pos);
//    Fifty move resets on capture
    pos->fifty_move = 0;
  }

//  Increment ply
  ++pos->hisPly;
  ++pos->ply;

  if(piecePwn[pos->pieces[from]]) {
//    Reset fifty move if a pawn is moved
    pos->fifty_move = 0;
//    Pawn start flag sets the en passant square
    if(move & PSFLAG) {
      if(side == WHITE) {
        pos->enPas = from + 10;
        assert(RanksBrd[pos->enPas] == RANK_3);
      } else {
        pos->enPas = from - 10;
        assert(RanksBrd[pos->enPas] == RANK_6);
      }
//      Update the key with en passant move
      pos->key ^= PieceKeys[EMPTY][pos->enPas];
    }
  }

//  Move the piece
  MovePiece(from, to, pos);

//  Get promoted piece
  int prPce = PROMOTED(move);
  if(prPce != EMPTY) {
    assert(PceValid(prPce));
//    Clear the promoted pawn
    ClearPiece(to, pos);
//    Add the promoted piece
    AddPiece(to, pos, prPce);
  }

//  Update king square if the piece is a king
  if(isK(pos->pieces[to]))
    pos->KingSq[pos->side] = to;

//  XOR the side key and switch sides
  pos->key ^= SideKey;
  pos->side ^= 1;


//  std::cout << "Move processed" << std::endl;

//  PrintBoard(pos);
//  If after making the move, the king square is attacked, then undo this move
  if(IsSqAttacked(pos->KingSq[side], pos->side, pos)) {
//    std::cout << "King attacked" << std::endl;
    TakeMove(pos);
    return false;
  }

//  std::cout << "Move done" << std::endl;
  return true;
}

void TakeMove(std::shared_ptr<BOARD> pos) {

//  std::cout << "Take move" << std::endl;
#ifdef DEBUG
  assert(CheckBoard(pos));
#endif

//  Decrement ply
  --pos->hisPly, --pos->ply;

//  Get the move to undo and its info from the position history array
  int move = pos->history[pos->hisPly].move;
  pos->fifty_move = pos->history[pos->hisPly].fiftymove;
  pos->castlePerm = pos->history[pos->hisPly].castlePerm;
  pos->enPas = pos->history[pos->hisPly].enPas;
  int from = FROMSQ(move);
  int to = TOSQ(move);

//  std::cout << PrSq(to) << PrSq(from) << std::endl;
  assert(OnBoard(from));
  assert(OnBoard(to));

//  Switch sides
  pos->side ^= 1;

//  Add pieces back if en passant flag is set
  if(move & EPFLAG) {
    if(pos->side == WHITE)
      AddPiece(to - 10, pos, bP);
    else
      AddPiece(to + 10, pos, wP);
  } else if(CSFLAG & move) {
//    Put rooks back to their position if the castling flag is set
    switch(to) {
      case C1 :
        MovePiece(D1, A1, pos);  break;
      case C8 :
        MovePiece(D8, A8, pos);  break;
      case G1 :
        MovePiece(F1, H1, pos);  break;
      case G8 :
        MovePiece(F8, H8, pos);  break;
      default :  assert(false); break;
    }
  }

//  Move the piece from the to to from square
  MovePiece(to, from, pos);

//  If it is a king then update the kings square
  if(isK(pos->pieces[from]))
    pos->KingSq[pos->side] = from;

//  Check if there was any captures in this move
  int captured = CAPTURED(move);
  if(captured != EMPTY) {
    assert(PceValid(captured));
//    Add captured piece back to the board
    AddPiece(to, pos, captured);
  }

//  Check if there was any promoted piece in this move
  int prPce = PROMOTED(move);
  if(prPce != EMPTY) {
    assert(PceValid(prPce) && !piecePwn[prPce]);
//    Clear the promoted piece, piece has moved backwards therefore clear from from
    ClearPiece(from, pos);
//    Add the promoted pawn back
    AddPiece(from, pos, pieceCol[prPce] == WHITE ? wP : bP);
  }

//  Get back the history key
  pos->key = pos->history[pos->hisPly].key;

#ifdef DEBUG
  assert(CheckBoard(pos));
#endif
//  PrintBoard(pos);
}