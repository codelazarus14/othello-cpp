#include <algorithm>
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

int selectMove(const MCNode& node, float c) {
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

std::vector<std::pair<size_t, int>> simTree(Othello& game, MCTree& tree, float c) {
  std::vector<std::pair<size_t, int>> kmAcc;

  // select a move, do it and update the game/accumulator
  auto pickMoveAndPush = [&](Othello& game, const MCNode& node, size_t key) {
      int moveIdx = selectMove(node, c);

      game = doMove(game, false, node.moves[moveIdx].first, node.moves[moveIdx].second);
      kmAcc.push_back({key, moveIdx});
  };

  while (!isGameOver(game)) {
    size_t key = game.getHashKey();
    MCNode node;
    // if key is already in tree, pick a new move
    try {
      node = tree.getHashTable().get(key);
      pickMoveAndPush(game, node, key);
    }
    // if we haven't seen it before, add node and stop the simulation
    catch (std::invalid_argument) {
      node = tree.insertNode(game, key);
      // pick a final move to do before returning
      pickMoveAndPush(game, node, key);
      break;
    }
  }
  
  // backUp will start from the final node added to the tree
  std::reverse(kmAcc.begin(), kmAcc.end());
  return kmAcc;
}

// pit two players against each other with different UCT search args
// verbose = whether or not to print out the entire game as it progresses
static void compete(int blackSims, float blackC, int whiteSims, int whiteC, bool verbose = false) {
  Othello game;
}

int main() {
  Othello o;
  std::vector<std::pair<size_t, int>> kmAcc = simTree(o, MCTree{o}, 2.0f);

  for (std::pair<size_t, int> keyMove : kmAcc) {
    std::cout << keyMove.first << ", " << keyMove.second << "\n";
  }
}