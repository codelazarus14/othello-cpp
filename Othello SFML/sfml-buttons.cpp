#include "sfml-buttons.h"

namespace OthelloSF {
	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (!m_isVisible) return;
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
		m_shadowRect.setSize({ buttonWidth + buttonHeight / 10, buttonHeight + buttonHeight / 10});
		m_shadowRect.setFillColor(sf::Color{ color.r, color.g, color.b, (sf::Uint8)(color.a / 2) });
		m_buttonRect.setSize({ buttonWidth, buttonHeight });
		m_buttonRect.setFillColor(color);

		// offset = 1/2 button size - 1/2 text size (or 0.8 (undoing *1.2 padding) * 0.5 = 0.4 button)
		// 1/2 (0.5) - 2/5 (0.4) = 0.1
		m_text.setOrigin(-buttonWidth * 0.1f, -buttonHeight * 0.1f);

		m_isHovered = false;
		m_active = false;
		m_isVisible = true;
	}

	sf::FloatRect Button::getGlobalBounds() {
		return m_shadowRect.getGlobalBounds();
	}

	sf::Vector2f Button::getPosition() {
		return m_buttonRect.getPosition();
	}

	void Button::setPosition(float x, float y) {
		m_buttonRect.setPosition(x, y);
		m_shadowRect.setPosition(x, y);
		m_text.setPosition(x, y);
	}

	void Button::onHoverEnter() {
		m_isHovered = true;
		m_buttonRect.setFillColor({ m_color.r, m_color.g, m_color.b, (sf::Uint8)(m_color.a / 1.5f) });
		m_shadowRect.setFillColor({ m_color.r, m_color.g, m_color.b, (sf::Uint8)(m_color.a / 2.5f) });
		m_text.setFillColor({ m_textColor.r, m_textColor.g, m_textColor.b, (sf::Uint8)(m_textColor.a / 1.5f) });
	}

	void Button::onHoverExit() {
		m_isHovered = false;
		m_buttonRect.setFillColor(m_color);
		m_shadowRect.setFillColor({ m_color.r, m_color.g, m_color.b, (sf::Uint8)(m_color.a / 2) });
		m_text.setFillColor(m_textColor);
	}

	void Button::onPress(const std::function<void()>& callback) {
		m_active = true;
		callback();
		sf::FloatRect bounds = m_buttonRect.getLocalBounds();
		sf::Vector2f pos = m_buttonRect.getPosition();
		m_buttonRect.setPosition(pos.x + bounds.height / 20, pos.y + bounds.height / 20);
	}

	void Button::onRelease(const std::function<void()>& callback) {
		m_active = false;
		callback();
		sf::FloatRect bounds = m_buttonRect.getLocalBounds();
		sf::Vector2f pos = m_buttonRect.getPosition();		
		m_buttonRect.setPosition(pos.x - bounds.height / 20, pos.y - bounds.height / 20);
	}
}