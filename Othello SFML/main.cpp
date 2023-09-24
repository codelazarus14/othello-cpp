#include <SFML/Graphics.hpp>
#include "mcts.h"

constexpr int g_boardScale{ 100 };
constexpr float g_boardPadding{ g_boardScale / 10 };
constexpr float g_squareOffset{ g_boardScale + g_boardPadding };

static sf::Color othelloGreen{ 0, 128, 0, 255 };
static std::vector<sf::RectangleShape> squares;
static std::vector<sf::CircleShape> pieces;

static void addPiece(sf::Color color, int posn) {
	sf::CircleShape piece{ g_boardScale / 2 };
	piece.setFillColor(color);
	piece.setPosition(g_boardPadding + g_squareOffset * (posn % 8),
					  g_boardPadding + g_squareOffset * (posn / 8));
	pieces.push_back(piece);
}

static void updatePiecesFromBoard(Othello& game) {
	for (int i = 0; i < 64; i++) {
		if (game.getBlackPieces()[i])
			addPiece(sf::Color::Black, i);
		else if (game.getWhitePieces()[i])
			addPiece(sf::Color::White, i);
	}
}

static void doMoveAndUpdateBoard(Othello& game, int blackSims, float blackC, int whiteSims, float whiteC) {
	pieces.clear();

	std::pair<int, int> move;
	if (game.getWhoseTurn() == Player::black) {
		std::cout << "\nBLACK'S TURN!\n";
		move = uctSearch(game, blackSims, blackC, false);
	}
	else {
		std::cout << "\nWHITE'S TURN!\n";
		move = uctSearch(game, whiteSims, whiteC, false);
	}
	doMove(game, false, move.first, move.second);

	updatePiecesFromBoard(game);
}

int main() {
	constexpr int dimensions{ static_cast<int>(8 * (g_boardScale + g_boardPadding) + g_boardPadding) };

	sf::RenderWindow window(sf::VideoMode(dimensions, dimensions), "Othello");

	for (int i = 0; i < 64; i++) {
		float xPos = g_boardPadding + g_squareOffset * (i % 8);
		float yPos = g_boardPadding + g_squareOffset * (i / 8);

		sf::RectangleShape square({ g_boardScale, g_boardScale });
		square.setFillColor(othelloGreen);
		square.setPosition(xPos, yPos);
		squares.push_back(square);
	}

	Othello game;
	updatePiecesFromBoard(game);
	bool resultPrinted = false;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::KeyReleased:
				if (event.key.scancode != sf::Keyboard::Scan::Escape)
					break;
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		window.clear();
		// draw the board
		for (sf::RectangleShape square : squares) window.draw(square);
		for (sf::CircleShape piece : pieces) window.draw(piece);
		window.display();

		if (!isGameOver(game))
			doMoveAndUpdateBoard(game, 1000, 2, 1000, 2);
		else if (!resultPrinted) {
			std::pair<int, int> counts{ game.getTotalPieces() };
			if (counts.first - counts.second > 0) {
				std::cout << "\nWhite wins!\n";
			}
			else if (counts.first - counts.second < 0) {
				std::cout << "\nBlack wins!\n";
			}
			else {
				std::cout << "\nTie!\n";
			}
			resultPrinted = true;
		}
	}

	return 0;
}