# othello-cpp
Othello MCTS implementation in C++ 11, supported by [SFML](https://www.sfml-dev.org/)'s graphical libraries.

This is the graphical demo of the Monte Carlo Tree Search algorithm on the Othello domain. For the CLI alternative, change to the `master` branch.

This version was created for Windows but could notionally be adapted to use other SFML-friendly environments.

# Usage

Build and run the Visual Studio project inside the `Othello SFML` directory.

Once loaded, the program should present the user with a screen asking them to choose "Human vs. CPU" or "CPU vs. CPU". The latter emulates the functionality of the original `master` code where two players are pitted against each other using the same algorithm. To change the values given to the MCTS algorithm (number of simulations, C-value), parameters can now be changed in `main.cpp` inside the `startNewGame` lambda of `onMouseReleased`.

"Human vs. CPU" allows you to play against the AI for either white or black, with the default option of showing legal move highlights set to `true` in the static variable `s_showLegal`. The game will continue until the player exits the game, uses the "Restart" button in the pause menu or the game is over.

~~Yes I should have put a proper settings menu in.~~
