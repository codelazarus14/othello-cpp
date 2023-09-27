#ifndef SFML_BUTTONS_H
#define SFML_BUTTONS_H

#include <functional>
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
		bool m_isHovered;
		bool m_active;
		bool m_isVisible;
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	public:
		Button(const sf::Font& font, const std::string& str, const sf::Color& color = sf::Color::White);
		sf::FloatRect getGlobalBounds();
		sf::Vector2f getPosition();
		sf::Text getText() { return m_text; }
		void setPosition(float x, float y);
		void setVisible(bool visible) { m_isVisible = visible; }
		bool isHovered() { return m_isHovered; }
		bool isActive() { return m_active; }
		bool isVisible() { return m_isVisible; }
		void onHoverEnter();
		void onHoverExit();
		void onPress(const std::function<void()>& callback);
		void onRelease(const std::function<void()>& callback);
	};
}

#endif