#include"validate.hpp"

bool OnBoard(const int sq) {
  return FilesBrd[sq] != OFF_BOARD;
}

bool SideValid(const int side) {
  return (side == WHITE || side == BLACK);
}

bool FRValid(const int fr) {
  return (fr >= 0 && fr <= 7);
}

bool PceValidEmpty(const int pce) {
  return (pce >= EMPTY && pce <= bK);
}

bool PceValid(const int pce) {
  return (pce >= wP && pce <= bK);
}