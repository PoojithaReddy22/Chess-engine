#include "search.hpp"
#include <algorithm>

static bool IsRepetition(std::shared_ptr<const BOARD> pos) {
//  If fifty moves resets then there is no going back since pawns cant go back or capture undone
  for(int idx = pos->hisPly - pos->fifty_move; idx < pos->hisPly - 1; ++idx) {
    assert(idx >= 0 && idx <= MAX_MOVES);
    if(pos->key == pos->history[idx].key)
      return true;
  }
  return false;
}

static void ClearForSearch(std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info) {
  for(int sq = 0; sq < BRDSQ_120; ++sq) {
    for(int *searchHistory : pos->searchHistory)
      searchHistory[sq] = 0;
  }

  for(int depth = 0; depth < MAXDEPTH; ++depth) {
    for(int *searchKiller : pos->searchKillers)
      searchKiller[depth] = 0;
  }

  pos->PvTable = std::make_unique<PVTABLE>(PVTABLE(PvSize));
  pos->ply = 0;

  info->start_time = GetTime();
  info->stop = false;
  info->nodes = 0;

  info->fh = 0;
  info->fhf = 0;
}

static int Quiescence(int alpha, int beta, std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info) {
  assert(CheckBoard(pos));

  ++info->nodes;

//  return 0 for repetition (draw)
  if(IsRepetition(pos) || pos->fifty_move >= 100)
    return 0;

  if(pos->ply >= MAXDEPTH)
    return EvalPosition(pos);

  int score = EvalPosition(pos);

  if(score >= beta)
    return beta;

  if(score > alpha)
    alpha = score;

  std::shared_ptr<MOVE_LIST> list = std::make_shared<MOVE_LIST>();
  list->count = 0;
  GenerateAllCaptureMoves(pos, list);

  int legal = 0;
  int best_move = NOMOVE;
  int old_alpha = alpha;
  int pv_move = ProbePvTable(pos);

  for(MOVE move : list->moves) {
    if(move.move == pv_move) {
      move.score = 2e6;
      break;
    }
  }

  list->SortMoves();

  for(int num = 0; num < list->count; ++num) {
    if(!MakeMove(pos, list->moves[num].move))
      continue;

    ++legal;
    score = -Quiescence(-beta, -alpha, pos, info);
    TakeMove(pos);

    if(score > alpha) {
//      score exceeds beta cutoff, higher player wont allow
      if(score >= beta) {
        if(legal == 1)
          ++info->fhf;

        ++info->fh;
        return beta;
      }

      alpha = score;
      best_move = list->moves[num].move;
    }
  }

//  store the pv move if the value of alpha is improved
  if(alpha != old_alpha)
    StorePvMove(pos, best_move);

  return alpha;
}

static int AlphaBeta(int alpha, int beta, int current_depth, std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info) {
  assert(CheckBoard(pos));

  if(current_depth == 0) {
    return Quiescence(alpha, beta, pos, info);
  }

  ++info->nodes;

//  return 0 for repetition (draw)
  if(IsRepetition(pos) || pos->fifty_move >= 100)
    return 0;

  if(pos->ply > MAXDEPTH - 1)
    return EvalPosition(pos);

  std::shared_ptr<MOVE_LIST> list = std::make_shared<MOVE_LIST>();
  GenerateAllMoves(pos, list);

  int legal = 0;
  int best_move = NOMOVE;
  int old_alpha = alpha;
  int pv_move = ProbePvTable(pos);

  for(MOVE move : list->moves) {
    if(move.move == pv_move) {
      move.score = 2e6;
      break;
    }
  }

  list->SortMoves();

  for(int num = 0; num < list->count; ++num) {
    if(!MakeMove(pos, list->moves[num].move))
      continue;

    ++legal;
    int score = -AlphaBeta(-beta, -alpha, current_depth - 1, pos, info);
    TakeMove(pos);

    if(score > alpha) {
//      score exceeds beta cutoff, higher player wont allow
      if(score >= beta) {
        if(legal == 1)
          ++info->fhf;

        ++info->fh;

        if(!CAPTURED(list->moves[num].move)) {
          pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
          pos->searchKillers[0][pos->ply] = list->moves[num].move;
        }

        return beta;
      }

      alpha = score;
      best_move = list->moves[num].move;

      if(!CAPTURED(list->moves[num].move)) {
        pos->searchHistory[pos->pieces[FROMSQ(best_move)]][TOSQ(best_move)] += current_depth;
      }
    }
  }

  if(legal == 0) {
//    if no legal move and the king square is attacked than we have a mate
    if(IsSqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos))
      return -MATE + pos->ply;
    return 0;
  }

//  store the pv move if the value of alpha is improved
  if(alpha != old_alpha)
    StorePvMove(pos, best_move);

  return alpha;
}

void SearchPosition(std::shared_ptr<BOARD> pos, std::shared_ptr<SEARCH> info) {
  int best_move = NOMOVE;
  int best_score = -INFINITE;
  int pvMoves = 0;

  ClearForSearch(pos, info);

  for(int depth = 1; depth <= info->depth; ++depth) {
    best_score = AlphaBeta(-INFINITE, INFINITE, depth, pos, info);
    pvMoves = GetPvLine(depth, pos);
//    first move of the pv array is the move that needs to be played
    best_move = pos->PvArray[0];

    std::cout << "Depth : " << depth << " Score : " << best_score << " Move : " <<
              PrMove(best_move) << " Nodes : " << info->nodes << std::endl;

    std::cout << "Pv : " << ' ';
    for(int num = 0; num < pvMoves; ++num) {
      std::cout << PrMove(pos->PvArray[num]) << ' ';
    }
    std::cout << std::endl;
    if(info->fh)
      std::cout << "Ordering : " << info->fhf / info->fh << std::endl;
  }
}
