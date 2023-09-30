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
std::string s_whoseTurn;
// B/W pair
// false = CPU, true = human
std::pair <bool, bool> s_players;
bool s_pickingColor{ false };
bool s_pickingMove{ false };
bool s_showLegal{ true };
std::vector<std::pair<int, int>> s_userLegalMoves;
std::pair<int, int> s_userSelectedMove;

enum class BoardState {
	Setup,
	Playing,
	Paused,
	Resetting,
	Exit
};
BoardState s_boardState;

void createOverlay() {
	// draw overlay for indicating window received Event::Closed
	s_overlay.setFillColor(sf::Color(0, 0, 0, 160));
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
	auto createButton = [](const std::string& str, int x, int y, bool visible, sf::Color color = sf::Color::White) {
		OthelloSF::Button btn{ s_othelloFont, str, color };
		sf::FloatRect bounds{ btn.getGlobalBounds() };
		btn.setPosition(x - bounds.width / 2, y - bounds.height / 2);
		btn.setVisible(visible);
		s_overlayUI.push_back(btn);
	};

	createButton("Reset Game", g_dimensions / 2, g_dimensions / 2, false);
	createButton("Human vs. CPU", g_dimensions / 2, g_dimensions / 2, true);
	createButton("CPU vs. CPU", g_dimensions / 2, g_dimensions / 1.75f, true);
	createButton("Black", g_dimensions / 1.8f, g_dimensions / 1.75f, false, sf::Color::Black);
	createButton("White", g_dimensions / 1.5f, g_dimensions / 1.75f, false);
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

	auto setTextProperties = [](sf::Text& text, const std::string& str, int size = g_textFontSize) {
		text.setFont(s_othelloFont);
		text.setCharacterSize(size);
		text.setStyle(sf::Text::Bold);
		text.setString(str);
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	};

	// draw overlay/text on sidebar
	sf::Text escKeyText;
	setTextProperties(escKeyText, "ESC = Pause", 40);
	escKeyText.setPosition(g_dimensions * 1.25f, escKeyText.getLocalBounds().height);
	sf::Text moveText;
	setTextProperties(moveText, s_whoseTurn + "'s Turn");
	moveText.setPosition(g_dimensions * 1.25f, g_dimensions / 8.0f);
	sf::Text overlayText;
	overlayText.setPosition(g_dimensions / 2.0f, g_dimensions / 4.0f);
	sf::Text pickColorText;
	setTextProperties(pickColorText, "Pick color:", 35);
	pickColorText.setPosition(g_dimensions / 2.5f, g_dimensions / 1.8f);

	switch (s_boardState) {
	case BoardState::Setup:
		setTextProperties(overlayText, "Othello");
		window.draw(s_overlay);
		window.draw(overlayText);
		if (s_pickingColor)
			window.draw(pickColorText);
		for (OthelloSF::Button button : s_overlayUI) window.draw(button);
		break;
	case BoardState::Playing:
		window.draw(escKeyText);
		window.draw(moveText);
		break;
	case BoardState::Paused:
		window.draw(moveText);
		setTextProperties(overlayText, "Paused");
		window.draw(s_overlay);
		window.draw(overlayText);
		for (OthelloSF::Button button : s_overlayUI) window.draw(button);
		break;
	case BoardState::Resetting:
		window.draw(moveText);
		setTextProperties(overlayText, "Resetting...");
		window.draw(s_overlay);
		window.draw(overlayText);
		break;
	case BoardState::Exit:
		window.draw(moveText);
		setTextProperties(overlayText, "Exiting...");
		window.draw(s_overlay);
		window.draw(overlayText);
		break;
	}

	window.display();
	window.setActive(false);
}

std::pair<int, int> getUserMove(const Othello& game, bool showLegal) {
	static int invalidPosn = 99;
	static sf::Color transColor = s_players.first ?
		sf::Color{ 0, 0, 0, 128 } :
		sf::Color{ 255, 255, 255, 128 };
	s_userLegalMoves = legalMoves(game);
	size_t nTemp{ s_userLegalMoves.size() };

	// skip user input if they can't make a move
	if (nTemp == 1 && isPass(s_userLegalMoves[0]))
		return s_userLegalMoves[0];

	// temporarily add "pieces" to board
	for (int i = 0; i < nTemp; i++) {
		std::pair<int, int> pos{ s_userLegalMoves[i] };
		int posn = pos.first * 8 + pos.second;
		addPiece((showLegal ? transColor : s_othelloGreen), posn, invalidPosn);
	}

	s_pickingMove = true;
	// todo: fix busy waiting
	while (s_pickingMove) {
		if (s_boardState == BoardState::Exit || s_boardState == BoardState::Resetting)
			return s_userLegalMoves[0];
	}

	// remove temp pieces
	for (int j = 0; j < nTemp; j++)
		s_pieces.pop_back();

	return s_userSelectedMove;

}

void gameLoop(Othello& game, int blackSims, float blackC, int whiteSims, float whiteC) {
	int previousMove{};
	// main loop
	while (!isGameOver(game) && s_boardState != BoardState::Exit && s_boardState != BoardState::Resetting) {
		if (s_boardState == BoardState::Paused) continue;

		const Player currPlayer = game.getWhoseTurn();
		bool isHuman = (currPlayer == Player::black && s_players.first)
			|| (currPlayer == Player::white && s_players.second);
		s_whoseTurn = currPlayer == Player::black ? "Black" : "White";
		// draw current board state
		updatePiecesFromBoard(game, previousMove);

		std::pair<int, int> move;
		if (isHuman) {
			move = getUserMove(game, s_showLegal);
			std::cout << "User move: " << move.first << ", " << move.second << std::endl;
			if (isPass(move)) std::cout << "PASS!" << std::endl;
		}
		else {
			if (game.getWhoseTurn() == Player::black) {
				std::cout << "\nBLACK'S TURN!\n";
				move = uctSearch(game, blackSims, blackC, false);
			}
			else {
				std::cout << "\nWHITE'S TURN!\n";
				move = uctSearch(game, whiteSims, whiteC, false);
			}
		}

		previousMove = move.first * 8 + move.second;
		doMove(game, false, move.first, move.second);
	}

	// debug print after game over
	if (s_boardState != BoardState::Exit && s_boardState != BoardState::Resetting) {
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
		if (!button.isVisible()) continue;

		sf::FloatRect bounds = button.getGlobalBounds();

		if (bounds.contains(mouseMove.x, mouseMove.y) && !button.isHovered())
			button.onHoverEnter();
		else if (!bounds.contains(mouseMove.x, mouseMove.y) && button.isHovered() && !button.isActive())
			button.onHoverExit();
	}
}

void onMousePressed(sf::Event::MouseButtonEvent mousePress) {
	if (mousePress.button == sf::Mouse::Left) {
		if (s_boardState == BoardState::Playing && s_pickingMove) {
			int idxOffset = s_pieces.size() - s_userLegalMoves.size();

			// check for user selected legal move
			for (int i = 0; i < s_userLegalMoves.size(); i++) {
				sf::CircleShape tempPiece = s_pieces[idxOffset + i];
				sf::FloatRect bounds = tempPiece.getGlobalBounds();

				if (bounds.contains(mousePress.x, mousePress.y)) {
					s_userSelectedMove = s_userLegalMoves[i];
					s_pickingMove = false;
					break;
				}
			}
		}
		else if (s_boardState == BoardState::Setup || s_boardState == BoardState::Paused) {
			for (OthelloSF::Button& button : s_overlayUI) {
				if (!button.isVisible()) continue;

				sf::FloatRect bounds = button.getGlobalBounds();

				if (bounds.contains(mousePress.x, mousePress.y) && !button.isActive()) {
					button.onPress([]() {});
				}
			}
		}
	}
}

void onMouseReleased(sf::Event::MouseButtonEvent mouseRelease, sf::RenderWindow& window, std::thread& gameThread, Othello& game) {
	OthelloSF::Button& resetButton = s_overlayUI[0];
	OthelloSF::Button& humanVsCPUBtn = s_overlayUI[1];
	OthelloSF::Button& cpuVsCPUBtn = s_overlayUI[2];
	OthelloSF::Button& pickBlackBtn = s_overlayUI[3];
	OthelloSF::Button& pickWhiteBtn = s_overlayUI[4];

	auto setPauseMenuUI = [&]() {
		resetButton.setVisible(true);
		humanVsCPUBtn.setVisible(false);
		cpuVsCPUBtn.setVisible(false);
		pickBlackBtn.setVisible(false);
		pickWhiteBtn.setVisible(false);
	};

	auto startNewGame = [&game, &gameThread]() {
		s_boardState = BoardState::Playing;
		game = Othello{};
		// start new game/logic thread
		gameThread = std::thread{ gameLoop, std::ref(game), 1000, 2, 1000, 2 };
	};

	if (mouseRelease.button == sf::Mouse::Left) {
		if (humanVsCPUBtn.isActive()) {
			humanVsCPUBtn.onRelease([&]() {
				s_pickingColor = true;

				sf::FloatRect bounds = cpuVsCPUBtn.getGlobalBounds();
				cpuVsCPUBtn.setPosition(g_dimensions / 2 - bounds.width / 2, g_dimensions / 1.5f - bounds.height / 2);
				pickBlackBtn.setVisible(true);
				pickWhiteBtn.setVisible(true);
			});
		}
		else if (pickBlackBtn.isActive()) {
			pickBlackBtn.onRelease([&]() {
				s_pickingColor = false;
				s_players = { true, false };
				setPauseMenuUI();
				startNewGame();
			});
		}
		else if (pickWhiteBtn.isActive()) {
			pickWhiteBtn.onRelease([&]() {
				s_pickingColor = false;
				s_players = { false, true };
				setPauseMenuUI();
				startNewGame();
			});
		}
		else if (cpuVsCPUBtn.isActive()) {
			cpuVsCPUBtn.onRelease([&]() {
				s_pickingColor = false;
				s_players = { false, false };
				setPauseMenuUI();
				startNewGame();
			});
		}
		else if (resetButton.isActive()) {
			resetButton.onRelease([&]() {
				s_boardState = BoardState::Resetting;
				renderBoard(window);
				if (gameThread.joinable())
					gameThread.join();

				startNewGame();
			});
		}
	}
}

void onKeyReleased(sf::Event::KeyEvent keyRelease) {
	// toggle pausing
	if (keyRelease.scancode == sf::Keyboard::Scan::Escape) {
		if (s_boardState == BoardState::Setup) return;

		s_boardState = s_boardState == BoardState::Paused ? BoardState::Playing : BoardState::Paused;
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

	// handle events
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				onWindowClosed(window, gameThread);
				break;
			case sf::Event::KeyReleased:
				onKeyReleased(event.key);
				break;
			case sf::Event::MouseMoved:
				onMouseMoved(event.mouseMove);
				break;
			case sf::Event::MouseButtonPressed:
				onMousePressed(event.mouseButton);
				break;
			case sf::Event::MouseButtonReleased:
				onMouseReleased(event.mouseButton, window, gameThread, game);
				break;
			}
		}

		renderBoard(window);
	}

	return 0;
}