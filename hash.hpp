#ifndef HASH_H
#define HASH_H

#include<cstdlib>
#include"board.hpp"

//Generates 64 bit random number
#define RAND_64 ( (U64)rand() | \
                  (U64)rand() << 15 | \
                  (U64)rand() << 30 | \
                  (U64)rand() << 45 | \
                  (U64)rand() << 60 )

//Unique value for each piece at each position
extern unsigned U64 PieceKeys[13][120];
//Unique value for the side that is playing currently
extern unsigned U64 SideKey;
//Unique value for each castling combination
extern unsigned U64 CastleKeys[16];
//EnPas pos is taken care by PieceKeys[EMPTY][pos->enPas]

//Initialize the hash to set the unique values
extern void InitHash();
//Generate key for given board state
extern unsigned U64 GeneratePosKey(std::shared_ptr<const BOARD> pos);

#endif