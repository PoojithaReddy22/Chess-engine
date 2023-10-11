#ifndef VALIDATE_H
#define VALIDATE_H

#include"board.hpp"

//Check if the square is on board
extern bool OnBoard(const int sq);
//Check if the side is valid
extern bool SideValid(const int side);
//Check if Files and Ranks are valid
extern bool FRValid(const int fr);
//Check if whether the piece is empty or valid
extern bool PceValidEmpty(const int pce);
//Check piece is not empty and valid
extern bool PceValid(const int pce);

#endif