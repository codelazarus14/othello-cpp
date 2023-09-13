#ifndef OTHELLO_H
#define OTHELLO_H

#include <iostream>
#include <utility>
#include <array>
#include <bitset>

constexpr int g_boardSize{8};

enum class Player {
  none,
  white,
  black
};

std::ostream& operator<< (std::ostream& out, const Player& player);

class Othello {
  private:
    // SIZE^2 array of spaces, initially unoccupied
    Player m_board[g_boardSize][g_boardSize]{ Player::none };
    // either black or white
    Player m_whoseTurn;
    // bits representing occupied positions on the board
    std::bitset<64> m_whitePieces, m_blackPieces;
    // number of unoccupied spaces, <= 64 - 4 starting pieces
    int m_numOpen;
  public:
    Othello();
    const Player (&getBoard())[g_boardSize][g_boardSize] { return m_board; }
    const Player& getWhoseTurn() { return m_whoseTurn; }
    const std::bitset<64>& getWhitePieces() { return m_whitePieces; }
    const std::bitset<64>& getBlackPieces() { return m_blackPieces; }
    const int& getNumOpen() { return m_numOpen; }
    // returns sum of (w, b) pieces on board
    const std::pair<int, int> getTotalPieces() const;
    // place b/w piece at position on board
    void placePiece(Player player, int pos);
    // get player at position (row, col)
    const Player& operator()(int row, int col);
    friend std::ostream& operator<<(std::ostream& out, const Othello& game);
};

#endif