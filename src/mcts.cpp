#include <algorithm>
#include "mcts.h"

const MCNode MCTree::insertNode(const Othello& game, size_t key) {
  std::vector<std::pair<int, int>> moves{legalMoves(game)};
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
    // if key is already in tree, pick a new move
    try {
      pickMoveAndPush(game, tree.getHashTable().get(key), key);
    }
    // if we haven't seen it before, add node and stop the simulation
    catch (std::invalid_argument) {
      // pick a final move to do before returning
      pickMoveAndPush(game, tree.insertNode(game, key), key);
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
      MCNode& node{hashy.get(key)};

      // update stats on each node from each key/move pair
      node.numVisits++;
      node.moveVisits[move]++;
      node.moveScores[move] += 
        (result - node.moveScores[move]) / node.moveVisits[move];
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
  MCNode root{tree.getRootNode()};
  // c=0: don't explore - just pick the best one
  int bestMove = selectMove(root, 0); 
  float bestScore = root.moveScores[bestMove];

  if (verbose) {
    std::cout << "Best score: " << bestScore;
    std::cout << ", scores: ";
    for (int i = 0; i < root.moveScores.size() - 1; i++)
      std::cout << root.moveScores[i] << ", ";
    std::cout << root.moveScores[root.moveScores.size() - 1];
    std::cout << "\nVisits: ";
    for (int visits : root.moveVisits) 
      std::cout << visits << " ";
    std::cout << "\n";
  }

  return root.moves[bestMove];
}

void compete(int blackSims, float blackC, int whiteSims, int whiteC, bool verbose) {
  Othello game;

  while (!isGameOver(game)) {
    std::pair<int, int> move;
    if (game.getWhoseTurn() == Player::black) {
      std::cout << "\nBLACK'S TURN!\n";
      move = uctSearch(game, blackSims, blackC, verbose);
    } else {
      std::cout << "\nWHITE'S TURN!\n";
      move = uctSearch(game, whiteSims, whiteC, verbose);
    }
    doMove(game, false, move.first, move.second);
    if (verbose) 
      std::cout << game;
  }

  // print final game board for non-verbose
  if (!verbose) {
    std::cout << "\n==========RESULT==========\n";
    std::cout << game;
  }
  std::pair<int, int> counts{game.getTotalPieces()};
  if (counts.first - counts.second > 0) {
    std::cout << "\nWhite wins!\n";
  } else if (counts.first - counts.second < 0) {
    std::cout << "\nBlack wins!\n";
  } else {
    std::cout << "\nTie!\n";
  }
}

int main() {
  compete(1000, 2, 1000, 2, true);
}