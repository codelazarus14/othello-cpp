#include "mcts.h"

const MCNode MCTree::insertNode(const Othello& game, size_t key) {
  std::vector<std::pair<int, int>> moves = legalMoves(game);
  // init key, whoseTurn, numVisits, moves vector
  // and manually resize other two vectors to match
  MCNode newNode{ key, game.getWhoseTurn(), 0, moves };
  newNode.moveVisits.resize(moves.size());
  newNode.moveScores.resize(moves.size());
  hashy.insert(key, newNode);
  return newNode;
}

std::ostream& operator<<(std::ostream& out, const MCNode& node) {
  out << "Key: " << node.key << "\n";
  out << "Whose turn: " << node.whoseTurn << "\n";
  out << "Visits: " << node.numVisits << "\n";
  out << "Moves:\n";
  for (int i = 0; i < node.moves.size(); i++) {
    out << "  [" << node.moves[i].first << ", " << node.moves[i].second << "]";
    out << " Visited: " << node.moveVisits[i] << ", Score: " << node.moveScores[i] << "\n";
  }
  return out;
}

const int selectMove(const MCNode& node, float c) {
  const Player& player = node.whoseTurn;
  const std::vector<std::pair<int, int>> moves = node.moves;
  int numMoves = moves.size();

  if (!numMoves) {
    std::ostringstream err;
    err << "No legal moves for node " << node.key << "!";
    throw std::out_of_range(err.str());
  } else if (numMoves == 1) {
    // return index of first (and only) move
    return 0;
  } else {
    float bestScore;
    int bestIndex;
    
    for (int i = 0; i < numMoves; i++) {
      float qValue = node.moveScores[i];
      // weight unexplored actions more
      float moveVisits = !node.moveVisits[i] ?
        g_posInfinityInverse : node.moveVisits[i];
      // weight unexplored nodes more
      float nodeVisits = !node.numVisits ?
        g_posInfinity : node.numVisits;
      // compute explore bonus
      float exploreBonus = c * sqrt(log(nodeVisits) / moveVisits);

      // update best score/index
      if (player == Player::black && qValue + exploreBonus > bestScore) {
        bestScore = qValue + exploreBonus;
        bestIndex = i;
      } else if (player == Player::white && qValue - exploreBonus < bestScore) {
        bestScore = qValue - exploreBonus;
        bestIndex = i;
      }
    }

    return bestIndex;
  }
}

// pit two players against each other with different UCT search args
// verbose = whether or not to print out the entire game as it progresses
static void compete(int blackSims, float blackC, int whiteSims, int whiteC, bool verbose = false) {
  Othello game;
}

int main() {
  Othello o;
  MCTree tree{o};

  for (int i = 0; i < 5; i++) {
    std::cout << "Move " << i << "\n";
    MCNode node = tree.insertNode(o, o.getHashKey());
    std::cout << node;
    int bestMove = selectMove(node, 2.0f);
    std::cout << bestMove;
    doMove(o, false, node.moves[bestMove].first, node.moves[bestMove].second);
    std::cout << o;
  }
}