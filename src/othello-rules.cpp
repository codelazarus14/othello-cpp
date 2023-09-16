#include <cassert>
#include <random>
#include "othello-rules.h"

// helper to determine if a given row/col move is legal for the game
static bool isLegal(const Othello& game, int row, int col) {
  // don't call this function with the "pass" move!
  assert(!isPass({row, col}));

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
  if (isPass({row, col})) {
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
          flippable.push_back({r, c});
          status = MoveCheckStatus::foundFlippable;
        }
      }
    }
  }
  // after flipping pieces, toggle player and return
  game.togglePlayer();
  return game;
}

const std::vector<std::pair<int, int>> legalMoves(const Othello& game) {
  std::vector<std::pair<int, int>> moves;
  // check every space for legal moves...
  for (int i = 0; i < g_boardSize; i++) {
    for (int j = 0; j < g_boardSize; j++) {
      if (isLegal(game, i, j)) {
        moves.push_back({i, j});
      }
    }
  }
  // if no legal moves, must pass
  if (!moves.size()) {
    moves.push_back(g_movePass);
  }
  return moves;
}

// helper to determine if a function has any legal move and stops early - faster than waiting for legalMoves
static bool hasLegalMove(const Othello& game) {
  for (int i = 0; i < g_boardSize; i++) {
    for (int j = 0; j < g_boardSize; j++) {
      if (isLegal(game, i, j)) return true;
    }
  }
  return false;
}

static bool mustPass(Othello& game, const Player& player) {
  // if it's our turn, just check
  if (player == game.getWhoseTurn())
    return !hasLegalMove(game);
  // if it's their turn, switch turns and check
  else {
    game.togglePlayer();
    bool answer = !hasLegalMove(game);
    game.togglePlayer();
    return answer;
  }
}

const bool isGameOver(Othello& game) {
  // no more open spaces or both players have no legal moves
  return game.getNumOpen() == 0
    || (mustPass(game, Player::black) && mustPass(game, Player::white));
}

static std::pair<int, int> randomMove(const Othello& game) {
  std::vector<std::pair<int, int>> moves = legalMoves(game);

	std::random_device rd;	// a seed source for the random number engine
	std::mt19937 gen(rd());	// mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(0, moves.size() - 1);

  return moves[distrib(gen)];
}

// helper used by defaultPolicy
static const Othello& doRandomMove(Othello& game) {
  // pick a random move and do it
  std::pair<int, int> randMove = randomMove(game);
  return doMove(game, false, randMove.first, randMove.second);
}

const float defaultPolicy(Othello& game) {
  while (!isGameOver(game)) {
    game = doRandomMove(game);
  }

  // compute score from pieces
  std::pair<int, int> counts = game.getTotalPieces();
  int white = counts.first;
  int black = counts.second;
  int diff = black - white;
  // W win = negative value
  if (diff < 0) return 0 - sqrt(abs(diff));
  // B win = positive value
  else if (diff > 0) return sqrt(diff);
  // tie (pieces equal) = 0
  else return 0;
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

  // std::cout << o;

  // std::cout << o.getNumOpen() << "\n";

  // std::cout << printBinary(o.getHashKey()) << "\n";

  // o.flipPiece(Player::black, toRow(27), toCol(27));

  // std::cout << o;

  // std::pair<int, int> move{4,5};
  // std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  // // std::cout << "  " << (isLegal(o, move.first, move.second) ? "Legal" :  "Illegal") << " move \n";
  // o = doMove(o, true, move.first, move.second);

  // std::cout << o;

  // move = {5, 5};
  // std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  // o = doMove(o, true, move.first, move.second);

  // std::cout << o;

  // move = {4, 2};
  // std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  // o = doMove(o, true, move.first, move.second);
  // move = {5, 4};
  // std::cout << "Trying move: " << move.first << ", " << move.second << "...\n";
  // o = doMove(o, true, move.first, move.second);

  // for (int i = 0; i < 10; i++) {
  //   std::cout << o;
    
  //   std::vector<std::pair<int, int>> moves = legalMoves(o);
  //   std::cout << "Legal moves:";
  //   for (std::pair<int, int> move : moves) {
  //     std::cout << " [" << move.first << ", " << move.second << "]";
  //   }
  //   std::cout << "\n";

  //   int r = std::rand() % moves.size();
  //   std::cout << "picking move " << r << "\n";
  //   o = doMove(o, true, moves[r].first, moves[r].second);
  // }

  // std::cout << (hasLegalMove(o) ? "True" : "False") << "\n";
  // std::cout << (mustPass(o, Player::white) ? "True" : "False") << "\n";
  // std::cout << (mustPass(o, Player::black) ? "True" : "False") << "\n";
  // std::cout << (isGameOver(o) ? "True" : "False") << "\n";

  float outcome = defaultPolicy(o);

  std::cout << "\nOutcome: " << outcome << "\n";

  std::cout << o;

  return 0;
}