#ifndef TEST_H
#define TEST_H

#include"board.hpp"
#include"move.hpp"

static void TestUtil(int depth, std::shared_ptr<BOARD> pos);
extern void Test(int depth, std::shared_ptr<BOARD> pos);

#endif