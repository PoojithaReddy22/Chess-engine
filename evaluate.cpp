#include "search.hpp"

const int PawnTable[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10,-20,-20, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0,
};

const int KnightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

const int BishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

const int RookTable[64] = {
    0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0,
};

const int QueenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
    0,  0,  5,  5,  5,  5,  0, -5,
    -5,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20,
};

const int KingMiddleGame[64] = {
    20, 30, 10,  0,  0, 10, 30, 20,
    20, 20,  0,  0,  0,  0, 20, 20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
};

const int KingEndGame[64] = {
    -50,-30,-30,-30,-30,-30,-30,-50,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -50,-40,-30,-20,-20,-30,-40,-50,
};

const int Mirror64[64] = {
    56,	57,	58,	59,	60,	61,	62,	63,
    48,	49,	50,	51,	52,	53,	54,	55,
    40,	41,	42,	43,	44,	45,	46,	47,
    32,	33,	34,	35,	36,	37,	38,	39,
    24,	25,	26,	27,	28,	29,	30,	31,
    16,	17,	18,	19,	20,	21,	22,	23,
    8,	9,	10,	11,	12,	13,	14,	15,
    0,	1,	2,	3,	4,	5,	6,	7
};

#define MIRROR64(sq) (Mirror64[(sq)])

static int Heuristic(int pce, int sq, bool endgame) {
//  heuristics are defined only for whites, mirror in the other case
  sq = SQ64(sq);
  if(pieceCol[pce] == BLACK)
    pce -= wK, sq = MIRROR64(sq);

  int pce_heuristic = 0;

  switch (pce) {
    case wP : pce_heuristic = PawnTable[sq]; break;
    case wN : pce_heuristic = KnightTable[sq]; break;
    case wB : pce_heuristic = BishopTable[sq]; break;
    case wR : pce_heuristic = RookTable[sq]; break;
    case wQ : pce_heuristic = QueenTable[sq]; break;
    case wK : pce_heuristic = (endgame ? KingEndGame[sq] : KingMiddleGame[sq]); break;
    default: break;
  }

  return pce_heuristic;
}

int EvalPosition(std::shared_ptr<const BOARD> pos) {
  bool endgame = pos->bigPce[WHITE] <= 5 || pos->bigPce[BLACK] <= 5;

  int score = pos->material[WHITE] - pos->material[BLACK];
  for(int pce = wP; pce <= bK; ++pce) {
    for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
      int sq = pos->pList[pce][pceNum];
      assert(OnBoard(sq));
      if(pieceCol[pce] == WHITE)
        score += Heuristic(pce, sq, endgame);
      else
        score -= Heuristic(pce, sq, endgame);
    }
  }

  if(pos->side == BLACK)
    return -score;

  return score;
}