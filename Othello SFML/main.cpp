#include <thread>
#include <functional>
#include <SFML/Graphics.hpp>
#include "mcts.h"
#include "sfml-buttons.h"

constexpr int g_boardScale{ 100 };
constexpr float g_boardPadding{ g_boardScale / 10 };
constexpr float g_squareOffset{ g_boardScale + g_boardPadding };
constexpr int g_dimensions{ static_cast<int>(8 * (g_boardScale + g_boardPadding) + g_boardPadding) };
constexpr int g_textFontSize{ 60 };

sf::Color s_othelloGreen{ 0, 128, 0, 255 };
sf::Color s_highlightColor{ 0, 0, 196, 255 };
sf::Font s_othelloFont;
sf::RectangleShape s_overlay{ {g_dimensions, g_dimensions} };
std::vector<sf::RectangleShape> s_squares;
std::vector<sf::CircleShape> s_pieces;
std::vector<OthelloSF::Button> s_overlayUI;

enum class BoardState {
	Setup,
	HumanTurn,
	CPUTurn,
	Paused,
	Exit
};
BoardState s_boardState;

void createOverlay() {
	// draw overlay for indicating window received Event::Closed
	s_overlay.setFillColor(sf::Color(0, 0, 0, 150));
}

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
	// s_UIButtons[0] = start button
	OthelloSF::Button startButton{ s_othelloFont, "Start" };
	sf::FloatRect startBounds{ startButton.getGlobalBounds() };
	startButton.setPosition(g_dimensions / 2 - startBounds.width / 2, g_dimensions / 2 - startBounds.height / 2);

	OthelloSF::Button resetButton{ s_othelloFont, "Reset" };
	sf::FloatRect resetBounds{ resetButton.getGlobalBounds() };
	resetButton.setPosition(g_dimensions / 2 - resetBounds.width / 2, g_dimensions / 1.5f - resetBounds.height / 2);

	s_overlayUI.push_back(startButton);
	s_overlayUI.push_back(resetButton);
}

// create drawable for board posn
void addPiece(sf::Color color, int posn, int previousMove) {
	sf::CircleShape piece{ g_boardScale / 2 };

	piece.setFillColor(color);
	piece.setPosition(g_boardPadding + g_squareOffset * (posn % 8),
		g_boardPadding + g_squareOffset * (posn / 8));
	// highlight previous move for visual clarity
	if (previousMove == posn) {
		piece.setOutlineThickness(8);
		piece.setOutlineColor(s_highlightColor);
	}
	s_pieces.push_back(piece);
}

// update s_pieces with board contents
void updatePiecesFromBoard(Othello& game, int previousMove) {
	const std::bitset<64> blackPieces = game.getBlackPieces();
	const std::bitset<64> whitePieces = game.getWhitePieces();

	s_pieces.clear();

	for (int i = 0; i < 64; i++) {
		if (blackPieces[i])
			addPiece(sf::Color::Black, i, previousMove);
		else if (whitePieces[i])
			addPiece(sf::Color::White, i, previousMove);
	}
}

void renderBoard(sf::RenderWindow& window) {
	window.setActive(true);
	window.clear();
	// draw the current board
	for (sf::RectangleShape square : s_squares) window.draw(square);
	for (sf::CircleShape piece : s_pieces) window.draw(piece);

	auto setTextProperties = [](sf::Text& text, const std::string& str) {
		text.setFont(s_othelloFont);
		text.setCharacterSize(g_textFontSize);
		text.setStyle(sf::Text::Bold);
		text.setString(str);
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	};

	sf::Text overlayText;
	overlayText.setPosition(g_dimensions / 2.0f, g_dimensions / 4.0f);
	sf::Text moveText;
	moveText.setPosition(g_dimensions * 1.25f, g_dimensions / 8.0f);

	switch (s_boardState) {
	case BoardState::Setup:
		setTextProperties(overlayText, "Othello");
		window.draw(s_overlay);
		window.draw(overlayText);
		break;
	case BoardState::CPUTurn:
		setTextProperties(moveText, "CPU Move...");
		window.draw(moveText);
		break;
	case BoardState::Paused:
		setTextProperties(overlayText, "Paused");
		window.draw(s_overlay);
		window.draw(overlayText);
		break;
	case BoardState::Exit:
		setTextProperties(overlayText, "Exiting...");
		window.draw(s_overlay);
		window.draw(overlayText);
		break;
	}

	// draw overlay UI
	for (OthelloSF::Button button : s_overlayUI) window.draw(button);

	window.display();
	window.setActive(false);
}

void cpuTurn(Othello& game, int blackSims, float blackC, int whiteSims, float whiteC) {
	// main loop
	int previousMove{};
	while (!isGameOver(game) && s_boardState == BoardState::CPUTurn) {
		// draw current board state
		updatePiecesFromBoard(game, previousMove);

		std::pair<int, int> move;
		if (game.getWhoseTurn() == Player::black) {
			std::cout << "\nBLACK'S TURN!\n";
			move = uctSearch(game, blackSims, blackC, false);
		}
		else {
			std::cout << "\nWHITE'S TURN!\n";
			move = uctSearch(game, whiteSims, whiteC, false);
		}
		previousMove = move.first * 8 + move.second;
		doMove(game, false, move.first, move.second);
	}

	// debug print after game over
	if (s_boardState != BoardState::Exit) {
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
	for (OthelloSF::Button& button : s_overlayUI) {
		sf::FloatRect bounds = button.getGlobalBounds();

		if (bounds.contains(mouseMove.x, mouseMove.y) && !button.isHovered())
			button.onHoverEnter();
		else if (!bounds.contains(mouseMove.x, mouseMove.y) && button.isHovered() && !button.isActive())
			button.onHoverExit();
	}
}

void onMousePressed(sf::Event::MouseButtonEvent mousePress) {
	if (mousePress.button == sf::Mouse::Left) {
		for (OthelloSF::Button& button : s_overlayUI) {
			sf::FloatRect bounds = button.getGlobalBounds();

			if (bounds.contains(mousePress.x, mousePress.y) && !button.isActive()) {
				button.onPress([]() {
					std::cout << "Button clicked!" << std::endl;
				});
			}
		}
	}
}

void onMouseReleased(sf::Event::MouseButtonEvent mouseRelease, Othello& game, std::thread& gameThread) {
	if (mouseRelease.button == sf::Mouse::Left) {
		for (OthelloSF::Button& button : s_overlayUI) {

			if (button.isActive()) {
				button.onRelease([&game, &gameThread]() {
					std::cout << "Button released!" << std::endl;
					if (s_boardState == BoardState::Setup) {
						s_boardState = BoardState::CPUTurn;

						// remove start button
						for (OthelloSF::Button& b : s_overlayUI) b.setVisible(false);

						// start game/logic thread
						gameThread = std::thread{ cpuTurn, std::ref(game), 1000, 2, 1000, 2 };
					}
				});
			}
		}
	}
}

void onWindowClosed(sf::RenderWindow& window, std::thread& gameThread) {
	s_boardState = BoardState::Exit;
	renderBoard(window);
	// wait for game thread to stop
	if (gameThread.joinable())
		gameThread.join();
	window.close();
}

int main() {
	if (!s_othelloFont.loadFromFile("PTSans-Regular.ttf")) {
		std::cout << "Error loading font file!" << std::endl;
		return -1;
	}

	sf::RenderWindow window(sf::VideoMode(static_cast<int>(g_dimensions * 1.5f), g_dimensions), "Othello");
	window.setActive(false);

	createOverlay();
	createBoardSquares();
	createBoardUI();

	Othello game;
	std::thread gameThread;

	renderBoard(window);

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
				onWindowClosed(window, gameThread);
				break;
				// check for mouse movement
			case sf::Event::MouseMoved:
				onMouseMoved(event.mouseMove);
				break;
			case sf::Event::MouseButtonPressed:
				onMousePressed(event.mouseButton);
				break;
			case sf::Event::MouseButtonReleased:
				onMouseReleased(event.mouseButton, game, gameThread);
				break;
			}
		}

		renderBoard(window);
	}

	return 0;
}