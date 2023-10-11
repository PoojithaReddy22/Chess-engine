#include"chess.hpp"

const std::string test_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const std::string mate_in_3 = "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1";

int main(int argc, char const *argv[]) {

  std::shared_ptr<BOARD> b(new BOARD);
  std::shared_ptr<SEARCH> s(new SEARCH);

  ParseFen(b, START_FEN);
  PrintBoard(b);

  while(true) {
    std::cout << "Enter a move > ";
    std::string input;
    std::cin >> input;
    if(input.empty()) {
      continue;
    } else if(input == "q") {
      break;
    } else if(input == "fen") {
      std::cout << "Enter fen string > ";
      std::string fen;
      std::cin.ignore();
      std::getline(std::cin, fen, '\n');
      ParseFen(b, fen);
      PrintBoard(b);
    } else if(input == "test") {
      std::cout << "Enter depth > ";
      int depth;
      std::cin >> depth;
      Test(depth,b);
    } else if(input == "t") {
      TakeMove(b);
      PrintBoard(b);
    } else if(input[0] == 's') {
      if(input[1] == '-') {
        s->depth = MAXDEPTH;
      } else {
        try {
          s->depth = std::stoi(input.substr(1));
        } catch (std::invalid_argument) {
          std::cout << "Invalid argument, enter again" << std::endl;
          continue;
        }
      }
      SearchPosition(b, s);
    } else {
      int move = ParseMove(b, input);
      if (move == NOMOVE) {
        std::cout << "Invalid move" << std::endl;
        continue;
      } else {
        StorePvMove(b, move);
        if(!MakeMove(b, move)) {
          std::cout << "Infeasible move" << std::endl;
          continue;
        }
      }
      StorePvMove(b, move);
      PrintBoard(b);
    }
  }

  return 0;
}
