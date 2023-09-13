#include <iostream>
#include <utility>
#include <array>

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

  m_whoseTurn = Player::black;
  m_numOpen = g_boardSize * g_boardSize - startingPieces.size();

  placePiece(Player::white, startingPieces[0]);
  placePiece(Player::white, startingPieces[1]);
  placePiece(Player::black, startingPieces[2]);
  placePiece(Player::black, startingPieces[3]);
}

const std::pair<int, int> Othello::getTotalPieces() const {
  std::pair<int, int> counts{0, 0};

  for (int r = 0; r < g_boardSize; r++) {
    for (int c = 0; c < g_boardSize; c++) {
      Player p = m_board[r][c];

      if (p == Player::white) counts.first++;
      else if (p == Player::black) counts.second++;
    }
  }
  return counts;
}

void Othello::placePiece(Player player, int pos) {

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
  std::pair<int, int> pieces = game.getTotalPieces();
  out << "  num-whites: " << pieces.first 
      << ", num-blacks: " << pieces.second << "\n";
      
  return out;
}

int main() {
  Othello o;
  std::cout << o;
  return 0;
}