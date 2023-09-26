#include <thread>
#include <functional>
#include <SFML/Graphics.hpp>
#include "mcts.h"
#include "sfml-buttons.h"

constexpr int g_boardScale{ 100 };
constexpr float g_boardPadding{ g_boardScale / 10 };
constexpr float g_squareOffset{ g_boardScale + g_boardPadding };
constexpr int g_dimensions{ static_cast<int>(8 * (g_boardScale + g_boardPadding) + g_boardPadding) };

sf::Color s_othelloGreen{ 0, 128, 0, 255 };
sf::Color s_highlightColor{ 0, 0, 196, 255 };
sf::Font s_othelloFont;
std::vector<sf::RectangleShape> s_squares;
std::vector<sf::CircleShape> s_pieces;
std::vector<OthelloSF::Button> s_UIButtons;
int s_previousMove;
bool s_windowClosing = false;

// create drawables for board
void createBoardSquares() {
	for (int i = 0; i < 64; i++) {
		float xPos = g_boardPadding + g_squareOffset * (i % 8);
		float yPos = g_boardPadding + g_squareOffset * (i / 8);

		sf::RectangleShape square({ g_boardScale, g_boardScale });
		square.setFillColor(s_othelloGreen);
		square.setPosition(xPos, yPos);
		s_squares.push_back(square);
	}
}

// create drawable UI elements
void createBoardUI() {
	OthelloSF::Button button{ s_othelloFont, "Another button" };
	button.setPosition(g_dimensions + g_boardPadding, g_dimensions / 2);

	OthelloSF::Button button2{ s_othelloFont, "A button" };
	button2.setPosition(g_dimensions + g_boardPadding, g_dimensions / 2 - button.getGlobalBounds().height * 2);

	s_UIButtons.push_back(button);
	s_UIButtons.push_back(button2);
}

// create drawable for board posn
void addPiece(sf::Color color, int posn) {
	sf::CircleShape piece{ g_boardScale / 2 };

	piece.setFillColor(color);
	piece.setPosition(g_boardPadding + g_squareOffset * (posn % 8),
					  g_boardPadding + g_squareOffset * (posn / 8));
	// highlight previous move for visual clarity
	if (s_previousMove == posn) {
		piece.setOutlineThickness(8);
		piece.setOutlineColor(s_highlightColor);
	}
	s_pieces.push_back(piece);
}

// update s_pieces with board contents
void updatePiecesFromBoard(Othello& game) {
	const std::bitset<64> blackPieces = game.getBlackPieces();
	const std::bitset<64> whitePieces = game.getWhitePieces();

	s_pieces.clear();

	for (int i = 0; i < 64; i++) {
		if (blackPieces[i])
			addPiece(sf::Color::Black, i);
		else if (whitePieces[i])
			addPiece(sf::Color::White, i);
	}
}

void renderBoard(sf::RenderWindow& window, bool exitOverlay) {
	window.setActive(true);
	window.clear();
	// draw the current board
	for (sf::RectangleShape square : s_squares) window.draw(square);
	for (sf::CircleShape piece : s_pieces) window.draw(piece);

	// draw UI
	for (OthelloSF::Button button : s_UIButtons) window.draw(button);

	if (exitOverlay) {
		// draw overlay for indicating window received Event::Closed
		sf::RectangleShape overlay{ {g_dimensions, g_dimensions} };
		overlay.setFillColor(sf::Color(0, 0, 0, 150));
		const int fontSize = 50;
		sf::Text overlayText;
		overlayText.setFont(s_othelloFont);
		overlayText.setString("Exiting...");
		overlayText.setCharacterSize(fontSize);
		overlayText.setStyle(sf::Text::Bold);
		overlayText.setOrigin(overlayText.getLocalBounds().width / 2, overlayText.getLocalBounds().height / 2);
		overlayText.setPosition(g_dimensions / 2.0f, g_dimensions / 4.0f);

		window.draw(overlay);
		window.draw(overlayText);
	}
	window.display();
	window.setActive(false);
}

void doMoveAndUpdateBoard(Othello& game, int blackSims, float blackC, int whiteSims, float whiteC,
	sf::RenderWindow& window, std::function<void(sf::RenderWindow&, bool)> renderFunc) {	
	// main loop
	while (!isGameOver(game) && !s_windowClosing) {
		// draw current board state
		updatePiecesFromBoard(game);

		std::pair<int, int> move;
		if (game.getWhoseTurn() == Player::black) {
			std::cout << "\nBLACK'S TURN!\n";
			move = uctSearch(game, blackSims, blackC, false);
		}
		else {
			std::cout << "\nWHITE'S TURN!\n";
			move = uctSearch(game, whiteSims, whiteC, false);
		}
		s_previousMove = move.first * 8 + move.second;
		doMove(game, false, move.first, move.second);
	}

	// debug print after game over
	if (!s_windowClosing) {
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
	}
}

void onMouseMoved(sf::Event::MouseMoveEvent mouseMove) {
	for (OthelloSF::Button& button : s_UIButtons) {
		sf::FloatRect bounds = button.getGlobalBounds();

		if (bounds.contains(mouseMove.x, mouseMove.y) && !button.isHovered())
			button.onHoverEnter();
		else if (!bounds.contains(mouseMove.x, mouseMove.y) && button.isHovered() && !button.isActive())
			button.onHoverExit();
	}
}

void onMousePressed(sf::Event::MouseButtonEvent mousePress) {
	if (mousePress.button == sf::Mouse::Left) {
		for (OthelloSF::Button& button : s_UIButtons) {
			sf::FloatRect bounds = button.getGlobalBounds();

			if (bounds.contains(mousePress.x, mousePress.y) && !button.isActive()) {
				button.onPress([]() {
					std::cout << "Button clicked!" << std::endl;
				});
			}
		}
	}
}

void onMouseReleased(sf::Event::MouseButtonEvent mouseRelease) {
	if (mouseRelease.button == sf::Mouse::Left) {
		for (OthelloSF::Button& button : s_UIButtons) {

			if (button.isActive()) {
				button.onRelease();
				std::cout << "Button released!" << std::endl;
			}
		}
	}
}

int main() {
	if (!s_othelloFont.loadFromFile("PTSans-Regular.ttf")) {
		std::cout << "Error loading font file!" << std::endl;
		return -1;
	}

	sf::RenderWindow window(sf::VideoMode(static_cast<int>(g_dimensions * 1.5f), g_dimensions), "Othello");
	window.setActive(false);

	createBoardSquares();
	createBoardUI();

	Othello game;

	// start game logic + render thread
	std::thread gameThread{ doMoveAndUpdateBoard, std::ref(game), 1000, 2, 1000, 2, std::ref(window), renderBoard };

	// handle events
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			// check for window close
			case sf::Event::KeyReleased:
				if (event.key.scancode != sf::Keyboard::Scan::Escape)
					break;
			case sf::Event::Closed:
				renderBoard(window, true);
				s_windowClosing = true;
				gameThread.join();
				window.close();
				break;
			// check for mouse movement
			case sf::Event::MouseMoved:
				onMouseMoved(event.mouseMove);
				break;
			case sf::Event::MouseButtonPressed:
				onMousePressed(event.mouseButton);
				break;
			case sf::Event::MouseButtonReleased:
				onMouseReleased(event.mouseButton);
				break;
			}

			renderBoard(window, false);
		}
	}

	return 0;
}