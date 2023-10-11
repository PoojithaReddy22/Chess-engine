# Chess-Engine

### Build
```
mkdir build
cd build
cmake ..
make -j4
./chess
```

### Input
```
fen - set the state of the board acc to the fen string
t - take back the current move
s{int_depth} - search the state tree to a given depth, 's-' implies infinite search
{move} - (DEFAULT) enter a move in the form FromTo square e.g. (h2h4), for promotions (h7h8q)
```

### To Do
- [ ] Add a chess board protocol (UCI maybe ?)
- [ ] Include other techniques like null move pruning, transposition tables, bitboards

PERFT Tests - 
https://www.chessprogramming.org/Perft_Results
