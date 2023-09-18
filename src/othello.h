#ifndef OTHELLO_H
#define OTHELLO_H

#include <iostream>
#include <utility>
#include <array>
#include <bitset>

// size/# of spaces in one dimension of the board
constexpr int g_boardSize{8};
// the "pass" move with OOB row/col
constexpr std::pair<int, int> g_movePass{99, 99};

// enum representing the types of pieces on the board
enum class Player {
  none,
  white,
  black
};

std::ostream& operator<< (std::ostream& out, const Player& player);

class Othello {
  private:
    // SIZE^2 array of spaces, initially unoccupied
    std::array<std::array<Player, g_boardSize>, g_boardSize> m_board{ Player::none };
    // either black or white
    Player m_whoseTurn;
    // bits representing occupied positions on the board
    std::bitset<64> m_whitePieces, m_blackPieces;
    // number of unoccupied spaces, <= boardSize - 4 starting pieces
    int m_numOpen;
  public:
    Othello();
    const std::array<std::array<Player, g_boardSize>, g_boardSize>& getBoard() { return m_board; }
    const std::array<std::array<Player, g_boardSize>, g_boardSize> getBoard() const { return m_board; }
    const Player& getWhoseTurn() { return m_whoseTurn; }
    const Player getWhoseTurn() const { return m_whoseTurn; }
    // swap w/b as current player
    void togglePlayer();
    const std::bitset<64>& getWhitePieces() { return m_whitePieces; }
    const std::bitset<64>& getBlackPieces() { return m_blackPieces; }
    // returns (w, b) sum of pieces on board
    const std::pair<int, int> getTotalPieces();
    const std::pair<int, int> getTotalPieces() const;
    int getNumOpen() { return m_numOpen; }
    // place piece at board[row, col] and update player bit vector
    void placePiece(const Player& player, int row, int col);
    // flip piece at board[row, col] and update both bit vectors
    void flipPiece(const Player& player, int row, int col);
    // get player at position (row, col)
    const Player& operator()(int row, int col);
    friend std::ostream& operator<<(std::ostream& out, const Othello& game);

    size_t getHashKey();
    size_t getHashKey() const;
};

// convert bit posn 0-boardSize to board row
inline int toRow(int posn) { return posn / g_boardSize; }
// convert bit posn 0-boardSize to board col
inline int toCol(int posn) { return posn % g_boardSize; }
// convert board [row, col] to bit posn 0-boardSize
inline int toPosn(int row, int col) { return row * g_boardSize + col; }
// determine whether move is = pass move constant
inline bool isPass(std::pair<int, int> move) { return move == g_movePass; }
// determine whether [row, col] is in bounds 
inline bool inBounds(int row, int col) { return row >= 0 && row < g_boardSize && col >= 0 && col < g_boardSize; }

#endif