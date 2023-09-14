#include <cassert>
#include "othello-rules.h"

bool isLegal(const Othello& game, int row, int col) {
  LegalMoveStatus status;
  Player player = game.getWhoseTurn();
  std::array<std::array<Player, g_boardSize>, g_boardSize> board = { game.getBoard() };

  // don't call this function with the "pass" move!
  assert(!isPass(std::pair<int, int>(row, col)));
  // picking an occupied position = always illegal
  if (board[row][col] != Player::none) return false;

  for (int i = 0; i < g_flipDirs.size(); i++) {
    status = LegalMoveStatus::lookingForFlippable;
    // get our direction values + current position
    int dr = g_flipDirs[i].first;
    int dc = g_flipDirs[i].second;
    int r = row;
    int c = col;

    while (status != LegalMoveStatus::illegal) {
      // advance one step
      r += dr;
      c += dc;

      // out of bounds = illegal (haven't found a matching end-piece)
      if (!inBounds(r, c)) {
        status = LegalMoveStatus::illegal;
      }
      else {
        // get piece at current position
        Player currPiece = board[r][c];
        // if we found a blank, or one of our own pieces before finding
        // one of the opponent's = illegal
        if (currPiece == Player::none || (status == LegalMoveStatus::lookingForFlippable && currPiece == player)) {
          status = LegalMoveStatus::illegal;
        }
        // if we found one of our own pieces after finding an opponent's
        // end of a flipping streak = legal 
        else if (currPiece == player) {
          return true;
        }
        // if we found an opponent's piece, we switch to looking for
        // our own (end of the streak)
        else {
          status = LegalMoveStatus::foundFlippable;
        }
      }
    }
  }
  // if we're here - we've exhausted all the move directions without
  // finding one that would make (row, col) legal
  return false; 
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

  std::pair<int, int> move{2,2};
  std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  std::cout << "  " << (isLegal(o, move.first, move.second) ? "Legal" :  "Illegal") << " move \n";

  return 0;
}