# othello-cpp
Othello MCTS implementation in C++ 11.

This is the CLI demo of the Monte Carlo Tree Search algorithm on the Othello domain. For the graphical alternative, change to the `sfml` branch.

# Usage

Compile all .cpp files in the `src` directory into a single executable and run. The program should immediately pit two AI players against each other with the parameters set for each player (num sims, c-value) defined in `mcts.cpp`'s main function. It should terminate whenever the game comes to an end (tie or a player wins).

The fifth parameter to `compete(...)` toggles whether the program displays verbose output (print board after every move).
