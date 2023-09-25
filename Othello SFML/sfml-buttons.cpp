#include "sfml-buttons.h"

namespace OthelloSF {
	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(m_shadowRect);
		target.draw(m_buttonRect);
		target.draw(m_text);
	}

	Button::Button(const sf::Font& font, const std::string& str, const sf::Color& color) : m_font{ font }, 
		m_color{ color }, m_textColor{ (sf::Uint8)(255 - color.r), (sf::Uint8)(255 - color.g), (sf::Uint8)(255 - color.b) } {
		m_text.setFont(font);
		m_text.setString(str);
		m_text.setFillColor(m_textColor);

		// padding
		float buttonWidth = m_text.getLocalBounds().width * 1.2f;
		float buttonHeight = m_text.getLocalBounds().height * 2.0f;
		m_shadowRect.setSize({ buttonWidth, buttonHeight });
		m_shadowRect.setFillColor(sf::Color{ color.r, color.g, color.b, (sf::Uint8)(color.a / 2) });
		m_buttonRect.setSize({ buttonWidth, buttonHeight });
		m_buttonRect.setFillColor(color);

		m_shadowRect.setOrigin(-buttonHeight / 10, -buttonHeight / 10);
		// offset = 1/2 button size - 1/2 text size (or 0.8 (undoing *1.2 padding) * 0.5 = 0.4 button)
		// 1/2 (0.5) - 2/5 (0.4) = 0.1
		m_text.setOrigin(-buttonWidth * 0.1f, -buttonHeight * 0.1f);
	}

	sf::FloatRect Button::getGlobalBounds() {
		return m_buttonRect.getGlobalBounds();
	}

	sf::Vector2f Button::getPosition() {
		return m_buttonRect.getPosition();
	}

	void Button::setPosition(float x, float y) {
		m_buttonRect.setPosition(x, y);
		m_shadowRect.setPosition(x, y);
		m_text.setPosition(x, y);
	}
}