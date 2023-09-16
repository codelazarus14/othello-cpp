#ifndef OTHELLO_RULES_H
#define OTHELLO_RULES_H

#include <vector>
#include "othello.h"

// the 4 up/down + 4 diagonal directions pieces can be flipped in
constexpr std::array<std::pair<int, int>, 8> g_flipDirs{
    std::pair<int, int>{1, 1},    // upper L - R
    std::pair<int, int>{1, 0},
    std::pair<int, int>{1, -1}, 
    std::pair<int, int>{0, 1},    // L - R
    std::pair<int, int>{0, -1},
    std::pair<int, int>{-1, 1},   // lower L - R
    std::pair<int, int>{-1, 0},
    std::pair<int, int>{-1, -1}
}; 

// enum to keep track of the state of our legal move search
enum class MoveCheckStatus
{
  lookingForFlippable,
  foundFlippable,
  doneChecking,
  illegal
};

// execute a move and returns the updated game
// if the move is illegal - prints an error and returns the unchanged game
const Othello& doMove(Othello& game, bool checkLegal, int row, int col);
// returns a list of the legal moves for the current game state
const std::vector<std::pair<int, int>> legalMoves(const Othello& game);
// game isn't a const reference bc another function toggles the current turn twice to see if neither side can make a move
const bool isGameOver(Othello& game);
// does random moves until the game is over
// and returns a score value = + for B win, - for W win
const float defaultPolicy(Othello& game);

#endif