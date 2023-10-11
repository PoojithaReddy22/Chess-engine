#ifndef BOARD_H
#define BOARD_H

#define U64 long long
#define BRDSQ_120 120
#define MAX_MOVES 2048

//  Convert from given file, rank to 120 board
#define FR2SQ(f, r) ((21 + (f)) + (r) * 10)
//  Convert from 120 board to 64 board
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
//  Convert from 64 board to 120 board
#define SQ120(sq64) (Sq64ToSq120[(sq64)])

#define MAXDEPTH 64

#include<string>
#include<algorithm>
#include<iostream>
#include<iomanip>
#include<memory>
#include<cassert>

//Sides
enum { WHITE, BLACK, BOTH };

//Defining position for 120 board
enum {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8,
  NO_SQ, OFF_BOARD
};

//Piece enums
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
//Rank enums
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };
//File enums
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
//Castling bit enums
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8};

extern const int PvSize;

//Principal variation table entry
struct PVENTRY {
  unsigned U64 key = {};
  int move = {};
};

//Principal Variation Table
struct PVTABLE {
  std::unique_ptr<PVENTRY[]> PvTable = {};
  U64 numEntries = {};
  PVTABLE(const int PvSize);
};

//Undo move structure
struct UNDO {
  int move = {};
  int castlePerm = {};
  int fiftymove = {};
  int enPas = {};
  unsigned U64 key = {};
};

//Board state representation
struct BOARD {
//  Board position to pieces mapping
  int pieces[BRDSQ_120] = {};
//  Side of the current player
  int side = {};
//  Castle permission bit
  int castlePerm = {};
//  Store the en-passant square, one before the played pawn
  int enPas = {};

//  count of half moves in the current search
  int ply = {};
//  history ply
  int hisPly = {};
  int fifty_move = {};
  unsigned U64 key = {};

//  Holds kings square
  int KingSq[2] = {};
//  Count of all pieces
  int pceNum[13] = {};
//  Count of all the big pieces
  int bigPce[2] = {};
//  Count of all the major pieces : rook, queen
  int majPce[2] = {};
//  Count of all the minor pieces : knight, bishop
  int minPce[2] = {};
//  Material of the two sides
  int material[2] = {};

  UNDO history[MAX_MOVES];
//  Mapping from all pieces to the 120 board
  int pList[13][10] = {};
//  Principal Variation Table
  std::unique_ptr<PVTABLE> PvTable;
  std::unique_ptr<int[]> PvArray;

  int searchHistory[13][BRDSQ_120];
  int searchKillers[2][MAXDEPTH];

  BOARD();
};

//Conversion utility
extern int Sq120ToSq64[BRDSQ_120];
extern int Sq64ToSq120[64];

//Return file and ranks corresponding to the square
extern int FilesBrd[BRDSQ_120];
extern int RanksBrd[BRDSQ_120];

//Mapping of the pieces to their value
extern int pieceBig[13];
extern int pieceMaj[13];
extern int pieceMin[13];
extern int pieceVal[13];
extern int pieceCol[13];
extern int piecePwn[13];

//Conversion array to chars
extern std::string PiceChar;
extern std::string SideChar;

//Initialize the conversion arrays
extern void InitBoard();

//Set the state of the board as empty
extern void ResetBoard(std::shared_ptr<BOARD> pos);

//Prints the conversion from 64 to 120 and vice versa
extern void PrintBoard();

//Print the pieces as present on the board
extern void PrintBoard(std::shared_ptr<BOARD> pos);

//Update evaluation material
extern void UpdateListMaterial(std::shared_ptr<BOARD> pos);

//Assertion utility
extern bool CheckBoard(std::shared_ptr<const BOARD> pos);

//Parse provided FEN string
extern void ParseFen(std::shared_ptr<BOARD> pos, const std::string fen);

//Parse input move
extern int ParseMove(std::shared_ptr<const BOARD> pos, std::string input);

//Get time since epoch
extern unsigned U64 GetTime();

#endif