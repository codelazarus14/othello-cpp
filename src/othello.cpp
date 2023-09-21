#include "othello.h"

std::ostream& operator<< (std::ostream& out, const Player& player) {
  switch (player) {
    case Player::none: out << "_"; break;
    case Player::white: out << "W"; break;
    case Player::black: out << "B"; break;
  }
  return out;
}

Othello::Othello() {
  std::array<int, 4> startingPieces{27, 36, 28, 35};

  // quick lambda so we don't have to overload Othello::placePiece
  auto setupPiece = [&](const Player& player, int posn) {
    m_board[toRow(posn)][toCol(posn)] = player;
  };

  m_whoseTurn = Player::black;
  m_numOpen = g_boardSize * g_boardSize - startingPieces.size();
  m_whitePieces = (1Ui64 << startingPieces[0]) + (1Ui64 << startingPieces[1]);
  m_blackPieces = (1Ui64 << startingPieces[2]) + (1Ui64 << startingPieces[3]);

  setupPiece(Player::white, startingPieces[0]);
  setupPiece(Player::white, startingPieces[1]);
  setupPiece(Player::black, startingPieces[2]);
  setupPiece(Player::black, startingPieces[3]);
}

void Othello::togglePlayer() {
  if (m_whoseTurn == Player::black) m_whoseTurn = Player::white;
  else if (m_whoseTurn == Player::white) m_whoseTurn = Player::black;
}

static std::pair<int, int> countTotal(const std::array<std::array<Player, g_boardSize>, g_boardSize>& board) {
  std::pair<int, int> counts{0, 0};

  for (int r = 0; r < g_boardSize; r++) {
    for (int c = 0; c < g_boardSize; c++) {
      const Player& p = board[r][c];

      if (p == Player::white) counts.first++;
      else if (p == Player::black) counts.second++;
    }
  }
  return counts;
}

const std::pair<int, int> Othello::getTotalPieces() {
  return countTotal(m_board);
}

const std::pair<int, int> Othello::getTotalPieces() const {
  return countTotal(m_board);
}

void Othello::placePiece(const Player& player, int row, int col) {
  m_board[row][col] = player;
  m_numOpen--;
  int pieceBit = toPosn(row, col);
  if (player == Player::black)
    m_blackPieces.set(pieceBit);
  else 
    m_whitePieces.set(pieceBit);
}

void Othello::flipPiece(const Player& player, int row, int col) {
  m_board[row][col] = player;
  int pieceBit = toPosn(row, col);
  if (player == Player::black) {
    m_blackPieces.set(pieceBit);
    m_whitePieces.reset(pieceBit);
  } else {
    m_whitePieces.set(pieceBit);
    m_blackPieces.reset(pieceBit);
  }
}

const Player& Othello::operator()(int row, int col) {
  return m_board[row][col];
}

std::ostream& operator<< (std::ostream& out, const Othello& game) {
  out << "\n |";
  for (int i = 0; i < g_boardSize; i++) { out << " " << i; }
  out << "\n------------------\n";
  for (int r = 0; r < g_boardSize; r++) {
    out << r << "| ";
    for (int c = 0; c < g_boardSize; c++) {
      out << game.m_board[r][c] << " ";
    }
    out << "\n";
  }
  out << "\nIt is " << game.m_whoseTurn << "'s turn!\n";
  out << "  white: " << game.m_whitePieces << "\n";
  out << "  black: " << game.m_blackPieces << "\n";
  // 
  std::pair<int, int> pieces{game.getTotalPieces()};
  out << "  num-white: " << pieces.first 
      << ", num-black: " << pieces.second << "\n";
      
  return out;
}

static size_t computeHash(std::bitset<64> white, std::bitset<64> black, Player whoseTurn) {
  size_t hashW = std::hash<std::bitset<64>>{}(white);
  size_t hashB = std::hash<std::bitset<64>>{}(black);
  size_t hashTurn = std::hash<int>{}(static_cast<int>(whoseTurn));
  // could have used boost::combine idk
  return hashW ^ (hashB << 1) ^ (hashTurn << 2);
}

size_t Othello::getHashKey() {
  return computeHash(m_whitePieces, m_blackPieces, m_whoseTurn);
}

size_t Othello::getHashKey() const {
  return computeHash(m_whitePieces, m_blackPieces, m_whoseTurn);
}