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
enum class LegalMoveStatus
{
  lookingForFlippable,
  foundFlippable,
  illegal
};

bool isLegal(const Othello &game, int row, int col);

#endif