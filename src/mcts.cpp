#include <algorithm>
#include "mcts.h"

const MCNode MCTree::insertNode(const Othello& game, size_t key) {
  std::vector<std::pair<int, int>> moves = legalMoves(game);
  // init key, whoseTurn, numVisits, moves vector
  // and manually resize other two vectors to match
  MCNode newNode{ key, game.getWhoseTurn(), 0, moves };
  newNode.moveVisits.resize(moves.size());
  newNode.moveScores.resize(moves.size());
  m_hashy.insert(key, newNode);
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
  int numMoves = node.moves.size();

  if (!numMoves) {
    std::ostringstream err;
    err << "No legal moves for node " << node.key << "!";
    throw std::out_of_range(err.str());
  } else if (numMoves == 1) {
    // return index of first (and only) move
    return 0;
  } else {
    float bestScore = node.moveScores[0];
    int bestIndex = 0;
    
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

void backUp(HashTable<MCNode>& hashy, std::vector<std::pair<size_t, int>> kmAcc, float result) {
  for (std::pair<size_t, int> keyMove : kmAcc) {
    size_t key = keyMove.first;
    int move = keyMove.second;

    try {
      // TODO: because i'm an idiot we can't edit the node's contents directly (.second of the KV pair)
      MCNode node{hashy.get(key)};

      // update stats on each node from each key/move pair
      node.numVisits++;
      node.moveVisits[move]++;
      node.moveScores[move] += 
        (result - node.moveScores[move]) / node.moveVisits[move];
      // wipe the old node, then add the new one
      hashy.remove(key);
      hashy.insert(key, node);
    }
      // shouldn't except if this key was either already in the table or added in the last iteration of simTree()
    catch (std::invalid_argument err) {
      std::cout << err.what() << "\n";
      std::cout << "This really shouldn't be happening in backUp!! Check kmAcc or something.";
    }
  }
}

std::pair<int, int> uctSearch(const Othello& origGame, int numSims, float c, bool verbose) {
  std::cout << "==========================\n";
  std::cout << "        UCT Search\n";
  std::cout << "==========================\n";
  
  MCTree tree{origGame};

  for (int i = 0; i < numSims; i++) {
    // clone the game and do a bunch of simulations
    Othello copy{origGame};
    std::vector<std::pair<size_t, int>> keyMoveAcc{simTree(copy, tree, c)};
    float result = simDefault(copy);

    backUp(tree.getHashTable(), keyMoveAcc, result);
  }
    
  // afterwards, find best move and print results
  MCNode root = tree.getRootNode();
  // c=0: don't explore - just pick the best one
  int bestMove = selectMove(root, 0); 
  float bestScore = root.moveScores[bestMove];

  if (verbose) {
    std::cout << "Best score: " << bestScore;
    std::cout << ", scores: ";
    for (float score : root.moveScores)
      std::cout << score << ", ";
    std::cout << "\nVisits: ";
    for (int visits : root.moveVisits) 
      std::cout << visits << " ";
    std::cout << "\n";
  }

  return root.moves[bestMove];
}

// pit two players against each other with different UCT search args
// verbose = whether or not to print out the entire game as it progresses
static void compete(int blackSims, float blackC, int whiteSims, int whiteC, bool verbose = false) {
  Othello game;
}

int main() {
  Othello o;
  float c = 2.0f;

  std::pair<int, int> bestMove{uctSearch(o, 1000, c, true)};
  std::cout << bestMove.first << ", " << bestMove.second << "\n";
}