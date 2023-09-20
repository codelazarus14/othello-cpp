#ifndef MCTS_H
#define MCTS_H

#include "othello.h"
#include "othello-rules.h"
#include "hash-table.h"

constexpr float g_posInfinity = 10000000;
constexpr float g_posInfinityInverse = 1 / g_posInfinity;

struct MCNode {
  size_t key;
  Player whoseTurn;
  int numVisits = 0;
  std::vector<std::pair<int, int>> moves;
  std::vector<int> moveVisits;
  std::vector<float> moveScores;
};

std::ostream& operator<<(std::ostream& out, const MCNode& node);

class MCTree {
  private:
    HashTable<MCNode> m_hashy{};
    size_t m_rootKey;
  public:
    // tree root state derived from game
    MCTree(const Othello& game) : m_rootKey(game.getHashKey()) {}
    HashTable<MCNode>& getHashTable() { return m_hashy; }
    const MCNode& getRootNode() { return m_hashy.get(m_rootKey); }
    // creates a new node and inserts it into the tree
    const MCNode insertNode(const Othello& game, size_t key);
};

// selects an index into the node's moves vector, weighing explored high-scoring actions against unexplored ones as determined by c (the exploitation-exploration constant)
// throws a std::out_of_range exception if there are no moves
int selectMove(const MCNode& node, float c);
// explores a path from the root node down to a yet-unexplored node and returns a list of (state, move) pairs
// state = a key into the tree's hash table of nodes
// move = an index into the moves vector of the corresponding node
std::vector<std::pair<size_t, int>> simTree(Othello& game, MCTree& tree, float c);
// explore a path using the default policy (random moves)
float simDefault(Othello& game) { return defaultPolicy(game); }
// update the relevant nodes in the hash table with a list of (key, move) accumulated pairs from a simulation run and the final score
void backUp(HashTable<MCNode>& hashy, std::vector<std::pair<size_t, int>> kmAcc, float result);
// uses the MCTS algorithm with the given parameters to estimate the best possible move for the current game state
std::pair<int, int> uctSearch(const Othello& origGame, int numSims, float C, bool verbose);

// pit two players against each other with different UCT search args
// verbose = whether or not to print out the entire game as it progresses
void compete(int blackSims, float blackC, int whiteSims, int whiteC, bool verbose);

#endif