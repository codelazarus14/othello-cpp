#include <SFML/Graphics.hpp>

constexpr int g_boardScale = 100;
constexpr int g_boardPadding = g_boardScale / 10;

int main() {
	constexpr int dimensions{ 8 * (g_boardScale + g_boardPadding) + g_boardPadding };
	constexpr int squareOffset{ g_boardScale + g_boardPadding };
	sf::Color othelloGreen{ 0, 128, 0, 255 };

	sf::RenderWindow window(sf::VideoMode(dimensions, dimensions), "Othello");
	std::vector <sf::RectangleShape> rects;
	for (int i = 0; i < 64; i++) {
		int xPos = g_boardPadding + squareOffset * (i % 8);
		int yPos = g_boardPadding + squareOffset * (i / 8);

		sf::RectangleShape square({g_boardScale, g_boardScale});
		square.setFillColor(othelloGreen);
		square.setPosition(xPos, yPos);
		rects.push_back(square);
	}

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
		for (int i = 0; i < rects.size(); i++) {
			window.draw(rects[i]);
		}
		window.display();
	}

	return 0;
}