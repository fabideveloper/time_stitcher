#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include "time.h"

using namespace sf;

class Obstacle {
public:
	RectangleShape m_shape;
	std::optional<Sprite> m_sprite;
	std::optional<Texture> m_texture;

	bool m_collidable = true;
	bool m_didTouch = false;
	Color m_fillColor = Color::White;

	// touch timer in seconds (non-blocking)
	float m_touchRemaining = 0.f;

	Obstacle(const Vector2f& position, const std::string& texturePath = "NULL") {
		if (!m_texture.emplace().loadFromFile(texturePath)) {
			m_shape.setPosition(position);
			m_shape.setSize({ 32.f, 32.f });
			m_fillColor = Color::Red;
			m_shape.setFillColor(m_fillColor);
		}
		else {
			m_texture->setSmooth(true);
			m_sprite.emplace(*m_texture);
			FloatRect bounds = m_sprite->getLocalBounds();
			m_sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
			m_sprite->setPosition(position);
		}
	}

	bool setTextureFromFile(const std::string& texturePath) {
		m_texture.emplace();
		if (!m_texture->loadFromFile(texturePath)) {
			m_texture.reset();
			return false;
		}
		m_texture->setSmooth(true);
		m_sprite.emplace(*m_texture);
		FloatRect bounds = m_sprite->getLocalBounds();
		m_sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
		if (m_shape.getSize() != Vector2f{ 0.f, 0.f }) {
			Vector2f center = m_shape.getPosition() + m_shape.getSize() * 0.5f;
			m_sprite->setPosition(center);
		}
		return true;
	}

	void draw(RenderWindow& window) {
		if (m_sprite) window.draw(*m_sprite);
		else window.draw(m_shape);
	}

	FloatRect getBounds() const {
		if (m_sprite) return m_sprite->getGlobalBounds();
		return m_shape.getGlobalBounds();
	}

	// Non-blocking: start brief "touched" visual state
	void touched() {
		// allow retrigger by resetting timer
		m_touchRemaining = 0.1f; // 100 ms
		m_didTouch = true;
		if (m_sprite) {
			m_sprite->setColor(Color::Yellow);
		}
		else {
			m_shape.setFillColor(Color::Yellow);
		}
	}

	// Called each frame from main loop with dt (seconds)
	void update(float dt) {
		if (m_touchRemaining <= 0.f) return;
		m_touchRemaining -= dt;
		if (m_touchRemaining <= 0.f) {
			// restore colors
			if (m_sprite) {
				m_sprite->setColor(Color::White); // white leaves texture unchanged
			}
			else {
				m_shape.setFillColor(m_fillColor);
			}
			m_didTouch = false;
			m_touchRemaining = 0.f;
		}
	}

	void setCollideable(bool collidable) {
		m_collidable = collidable;
		if (collidable) {
			if (!m_sprite) m_shape.setFillColor(Color::Red);
			else m_sprite->setColor(Color::White);
		}
		else {
			if (!m_sprite) m_shape.setFillColor(Color(100, 100, 100));
			else m_sprite->setColor(Color(150, 150, 150));
		}
	}

	bool intersects(const FloatRect& other) const {
		if (!m_collidable) return false;
		return getBounds().findIntersection(other) != std::nullopt;
	}
};