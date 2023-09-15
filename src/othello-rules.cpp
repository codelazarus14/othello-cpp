#include <cassert>
#include <vector>
#include "othello-rules.h"

bool isLegal(const Othello& game, int row, int col) {
  // don't call this function with the "pass" move!
  assert(!isPass(std::pair<int, int>(row, col)));

  MoveCheckStatus status;
  Player player = game.getWhoseTurn();
  std::array<std::array<Player, g_boardSize>, g_boardSize> board = game.getBoard();
  // picking an occupied position = always illegal
  if (board[row][col] != Player::none) return false;

  for (int i = 0; i < g_flipDirs.size(); i++) {
    status = MoveCheckStatus::lookingForFlippable;
    // get our direction values + current position
    int dr = g_flipDirs[i].first;
    int dc = g_flipDirs[i].second;
    int r = row;
    int c = col;

    while (status != MoveCheckStatus::illegal) {
      // advance one step
      r += dr;
      c += dc;

      // out of bounds = illegal (haven't found a matching end-piece)
      if (!inBounds(r, c)) {
        status = MoveCheckStatus::illegal;
      } else {
        // get piece at current position
        Player currPiece = board[r][c];
        // if we found a blank, or one of our own pieces before finding
        // one of the opponent's = illegal
        if (currPiece == Player::none || (status == MoveCheckStatus::lookingForFlippable && currPiece == player)) {
          status = MoveCheckStatus::illegal;
        }
        // if we found one of our own pieces after finding an opponent's
        // end of a flipping streak = legal 
        else if (currPiece == player) {
          return true;
        }
        // if we found an opponent's piece, we switch to looking for
        // our own (end of the streak)
        else {
          status = MoveCheckStatus::foundFlippable;
        }
      }
    }
  }
  // if we're here - we've exhausted all the move directions without
  // finding one that would make (row, col) legal
  return false; 
}

const Othello& doMove(Othello& game, bool checkLegal, int row, int col) {
  if (checkLegal && !isLegal(game, row, col)) {
    std::cout << "\n!! Not gonna do an illegal move!\n";
    return game;
  }
  // passes shouldn't modify the game except for whose turn it is
  if (isPass(std::pair<int, int>{row, col})) {
    game.togglePlayer();
    return game;
  }

  MoveCheckStatus status;
  Player player = game.getWhoseTurn();
  std::array<std::array<Player, g_boardSize>, g_boardSize> board = game.getBoard();

  // place piece on the board
  game.placePiece(player, row, col);

  for (int i = 0; i < g_flipDirs.size(); i++) {
    status = MoveCheckStatus::lookingForFlippable;
    // get our direction values + current position
    int dr = g_flipDirs[i].first;
    int dc = g_flipDirs[i].second;
    int r = row;
    int c = col;
    std::vector<std::pair<int, int>> flippable;

    while (status != MoveCheckStatus::doneChecking) {
      // advance
      r += dr;
      c += dc;

      // OOB - can't flip in this direction
      if (!inBounds(r, c)) {
        status = MoveCheckStatus::doneChecking;
      } else {
        Player currPiece = board[r][c];
        // blank/found one of ours before theirs - stop looking in this dir
        if (currPiece == Player::none || (status == MoveCheckStatus::lookingForFlippable && currPiece == player)) {
          status = MoveCheckStatus::doneChecking;
        }
        // found another of our own pieces, surrounding a line of flippables - flip them!
        else if (currPiece == player) {
          for (int j = 0; j < flippable.size(); j++) {
            game.flipPiece(player, flippable[j].first, flippable[j].second);
          }
          status = MoveCheckStatus::doneChecking;
        }
        // found one of their pieces to flip - keep looking
        else {
          flippable.push_back(std::pair<int, int>{r, c});
          status = MoveCheckStatus::foundFlippable;
        }
      }
    }
  }
  // after flipping pieces, toggle player and return
  game.togglePlayer();
  return game;
}

// helper for debugging
static std::string printBinary(const uint64_t& number) {
  int counter = 0;
  uint64_t binary = number;
  std::string out;

  while (counter < 64) {
    if (binary & 1)
      out.push_back('1');
    else 
      out.push_back('0');
    binary >>= 1;
    counter++;
    if (counter % 8 == 0 && counter != 64)
      out.push_back('_');
  }
  std::reverse(out.begin(), out.end());
  return out;
}

int main() {
  Othello o;

  std::cout << o;

  // std::cout << o.getNumOpen() << "\n";

  // std::cout << printBinary(o.getHashKey()) << "\n";

  // o.flipPiece(Player::black, toRow(27), toCol(27));

  // std::cout << o;

  std::pair<int, int> move{4,5};
  std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  // std::cout << "  " << (isLegal(o, move.first, move.second) ? "Legal" :  "Illegal") << " move \n";
  o = doMove(o, true, move.first, move.second);

  std::cout << o;

  move = {5, 5};
  std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  o = doMove(o, true, move.first, move.second);

  std::cout << o;

  move = {4, 2};
  std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  o = doMove(o, true, move.first, move.second);
  move = {5, 4};
  std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  o = doMove(o, true, move.first, move.second);

  std::cout << o;

  return 0;
}