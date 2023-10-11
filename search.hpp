#ifndef SEARCH_H
#define SEARCH_H

#define INFINITE 30000
#define MATE 29000

#include "board.hpp"
#include "move.hpp"
#include "validate.hpp"

struct SEARCH {
  unsigned U64 start_time;
  unsigned U64 stop_time;
  int depth;
  int depth_set;
  int time_set;
  int moves_to_go;
  bool infinite;
  U64 nodes;
  bool quit;
  bool stop;

//  fail high
  double fh;
//  fail high first
  double fhf;
};

//Return the Heuristic value of the piece according to its position
static int Heuristic(int pce, int sq, bool endgame);
//Return material evaluation
extern int EvalPosition(std::shared_ptr<const BOARD>);

//Check if time is up
static void CheckTime();
//Check for repetition of the same position
static bool IsRepetition(std::shared_ptr<const BOARD> pos);

//Clear the board and search
static void ClearForSearch(std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info);
//Alpha Beta Pruning
static int AlphaBeta(int alpha, int beta, int current_depth, std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info);
//Quiescence search for removing horizon effect
static int Quiescence(int alpha, int beta, std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info);
//Use iterative deepening for search
extern void SearchPosition(std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info);

#endif
