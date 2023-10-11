#include "board.hpp"
#include "move.hpp"

const int PvSize = 0x100000 * 2;

PVTABLE::PVTABLE(const int PvSize) {
  numEntries = PvSize / sizeof(PVENTRY);
  numEntries -= 2;
  PvTable = std::unique_ptr<PVENTRY[]>(new PVENTRY[numEntries * sizeof(PVENTRY)]);
  std::cout << "PV Table initialized with : " << numEntries << std::endl;
}

void StorePvMove(std::shared_ptr<BOARD> pos, const int move) {
  int idx = pos->key % pos->PvTable->numEntries;
  assert(idx >= 0 && idx < pos->PvTable->numEntries);
  pos->PvTable->PvTable[idx].move = move;
  pos->PvTable->PvTable[idx].key = pos->key;
}

int ProbePvTable(std::shared_ptr<const BOARD> pos) {
  int idx = pos->key % pos->PvTable->numEntries;
  assert(idx >= 0 && idx < pos->PvTable->numEntries);
  if(pos->PvTable->PvTable[idx].key == pos->key)
    return pos->PvTable->PvTable[idx].move;
  return NOMOVE;
}

bool MoveExists(std::shared_ptr<BOARD> pos, const int move) {
  std::shared_ptr<MOVE_LIST> list(new MOVE_LIST);
  GenerateAllMoves(pos, list);
  for(int idx = 0; idx < list->count; ++idx) {
    if(!MakeMove(pos, list->moves[idx].move)) {
      continue;
    }
    TakeMove(pos);
    if(list->moves[idx].move == move)
      return true;
  }
  return false;
}

int GetPvLine(const int depth, std::shared_ptr<BOARD> pos) {
  assert(depth < MAXDEPTH);

  int move = ProbePvTable(pos);
  int count = 0;

  while(move != NOMOVE && count < depth) {
    assert(count < MAXDEPTH);
    if(MoveExists(pos, move)) {
      MakeMove(pos, move);
      pos->PvArray[count++] = move;
    } else {
      break;
    }
    move = ProbePvTable(pos);
  }

  while(pos->ply > 0) {
    TakeMove(pos);
  }

  return count;
}