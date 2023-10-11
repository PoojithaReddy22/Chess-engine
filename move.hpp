#ifndef MOVES_H
#define MOVES_H

#include"board.hpp"
#include"validate.hpp"

/******************** MOVE INT DEFINITION *******************************
 * 0000 0000 0000 0000 0000 0111 1111 -> From
 * 0000 0000 0000 0011 1111 1000 0000 -> To
 * 0000 0000 0011 1100 0000 0000 0000 -> Captured piece type
 * 0000 0000 0100 0000 0000 0000 0000 -> En Passant capture move
 * 0000 0000 1000 0000 0000 0000 0000 -> Pawn start move, sets en passant
 * 0000 1111 0000 0000 0000 0000 0000 -> Promoted piece type
 * 0001 0000 0000 0000 0000 0000 0000 -> Castle move
 */

//Utility macro for checking the piece type
#define isP(pce) ((pce) == wP || (pce) == bP)
#define isN(pce) ((pce) == wN || (pce) == bN)
#define isB(pce) ((pce) == wB || (pce) == bB)
#define isR(pce) ((pce) == wR || (pce) == bR)
#define isQ(pce) ((pce) == wQ || (pce) == bQ)
#define isK(pce) ((pce) == wK || (pce) == bK)

//Check flags for en passant, pawn start, castling
#define EPFLAG 0x40000
#define PSFLAG 0x80000
#define CSFLAG 0x1000000

#define MAXPOSITIONMOVES 256

//Convert from, to, captured, promoted, flag(above) to move int
#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
//Check if the square is off board
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFF_BOARD)

//Extract meaning from move int
#define FROMSQ(m)   ((m) & 0x7F)
#define TOSQ(m)     (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

//Any illegal move
#define NOMOVE 0

//Defines a move for a game
struct MOVE{
//  Move int that stores the info of a move
  int move = {};
//  Move score for move ordering
  int score = {};

  inline bool operator> (const MOVE &m) const;
};

//List of moves
struct MOVE_LIST{
//  Array storing the moves
  MOVE moves[MAXPOSITIONMOVES] = {};
//  Count of moves in the array
  int count = {};

  void SortMoves();
  MOVE_LIST();
};

//FEN string for the start of the game
extern const std::string START_FEN;

//Convert from 120 to sq (e.g. 21 to A1)
extern std::string PrSq(const int sq);
//Print move in format FsqTsqPr
extern std::string PrMove(const int move);
//Print move list
void PrintMoveList(std::shared_ptr<const MOVE_LIST> list);

//Check if the current square is attacked by side
extern bool IsSqAttacked(const int sq, const int side, std::shared_ptr<const BOARD> pos);

/****************** MOVE TYPES *******************
 * Quiet
 * Capture
 * En passant
 */

static void AddQuietMove(int move, std::shared_ptr<MOVE_LIST> list);
static void AddCaptureMove(int move, std::shared_ptr<MOVE_LIST> list, std::shared_ptr<const BOARD> pos);
static void AddEnPassantMove(int move, std::shared_ptr<MOVE_LIST> list);

/******************** PAWN MOVES ********************
 *Pawn moves are special due to promotion
 * Quiet
 * Capture
 */

static void AddPawnMove(std::shared_ptr<const BOARD> pos, const int from, const int to, const int cap, const int side, std::shared_ptr<MOVE_LIST> list);
static void AddPawnMove(std::shared_ptr<const BOARD> pos, const int from, const int to, const int side, std::shared_ptr<MOVE_LIST> list);

//Loop all pieces and add all the possible moves corresponding to each to move list
extern void GenerateAllQuietMoves(std::shared_ptr<const BOARD> pos, std::shared_ptr<MOVE_LIST> list);
extern void GenerateAllCaptureMoves(std::shared_ptr<const BOARD> pos, std::shared_ptr<MOVE_LIST> list);
extern void GenerateAllMoves(std::shared_ptr<const BOARD> pos, std::shared_ptr<MOVE_LIST> list);

//Clear the piece, update key, material, minPce, majPce, bigPce, pList, pieces, pceNum
static void ClearPiece(const int sq, std::shared_ptr<BOARD> pos);

//Add piece, update key, material, majPce, minPce, bigPce, pList, pieces, pceNum
static void AddPiece(const int sq, std::shared_ptr<BOARD> pos, const int pce);

//Move piece, update key, pList, pieces
static void MovePiece(const int from, const int to, std::shared_ptr<BOARD> pos);

//Values of CastlePerm for each sq
static int CastlePerm(int sq);

//Takes move and updates the board structure
extern bool MakeMove(std::shared_ptr<BOARD> pos, int move);

//Undo a move
extern void TakeMove(std::shared_ptr<BOARD> pos);

//Store principal variation move
extern void StorePvMove(std::shared_ptr<BOARD> pos, const int move);
//Check if given move exists for the position
extern bool MoveExists(std::shared_ptr<BOARD> pos, const int move);

//Retrieve principal variation move
extern int ProbePvTable(std::shared_ptr<const BOARD> pos);
//Fill the PV Array after probing the PV Table
extern int GetPvLine(const int depth, std::shared_ptr<BOARD> pos);

//Initialise Most valuable victim Least valuable attacker array
static void InitMvvLva();

#endif
