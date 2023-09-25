#ifndef SFML_BUTTONS_H
#define SFML_BUTTONS_H

#include <SFML/Graphics.hpp>

namespace OthelloSF {
	class Button : public sf::Drawable {
	private:
		sf::RectangleShape m_shadowRect;
		sf::RectangleShape m_buttonRect;
		sf::Text m_text;
		sf::Font m_font;
		sf::Color m_color;
		sf::Color m_textColor;
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	public:
		Button(const sf::Font& font, const std::string& str, const sf::Color& color = sf::Color::White);
		sf::FloatRect getGlobalBounds();
		sf::Vector2f getPosition();
		void setPosition(float x, float y);
	};
}

#endif