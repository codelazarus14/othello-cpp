#ifndef OTHELLO_RULES_H
#define OTHELLO_RULES_H

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

// determine if a given row/col move is legal for the game
bool isLegal(const Othello& game, int row, int col);
// execute a move and returns the updated game
// if the move is illegal - prints and error and returns the unchanged game
const Othello& doMove(Othello& game, bool checkLegal, int row, int col);

#endif