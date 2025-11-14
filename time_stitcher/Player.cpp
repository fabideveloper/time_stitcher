#include "Player.h"
#include "Obstacle.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Player::Player(const std::string& texturePath, const Vector2f& startPos, float speed)
: m_speed(speed), m_loaded(false)
{
    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load " << texturePath << '\n';
        return;
    }

    m_texture.setSmooth(true);
        // construct the sprite only after the texture was successfully loaded
    m_sprite.emplace(m_texture);

    // center origin so rotation/positioning are intuitive
    FloatRect bounds = m_sprite->getLocalBounds();
    m_sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
    // m_sprite.setOrigin({ 0,0 });
    m_sprite->setPosition(startPos);

    m_loaded = true;
}

Vector2f Player::getPosition() const {
    if (m_sprite) return m_sprite->getPosition();
    return { 0.f, 0.f };
}

FloatRect Player::getBounds() const {
    if (m_sprite) return m_sprite->getGlobalBounds();
    // fallback: empty rect at current position
    Vector2f p = getPosition();
    return FloatRect({ p.x, p.y}, { 0.f, 0.f});
}

bool Player::isLoaded() const {
    return m_loaded;
}

void Player::setPosition(const Vector2f& pos) {
    if (m_sprite) m_sprite->setPosition(pos);
}

void Player::update(float dt, const Vector2u& windowSize) {
    if (!m_loaded) return;

    Vector2f dir(0.f, 0.f);
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) dir.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) dir.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) dir.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) dir.x += 1.f;

    if (dir.x != 0.f or dir.y != 0.f) {
        float len = std::hypot(dir.x, dir.y);
        if (len != 0.f) {
            dir.x /= len;
            dir.y /= len;
        }
        Vector2f pos = m_sprite->getPosition();
        pos += dir * m_speed * dt;

        // clamp so sprite stays fully inside the window
        FloatRect bounds = m_sprite->getLocalBounds();
        float halfW = bounds.size.x * 0.5f;
        float halfH = bounds.size.y * 0.5f;
        pos.x = std::clamp(pos.x, halfW, static_cast<float>(windowSize.x) - halfW);
        pos.y = std::clamp(pos.y, halfH, static_cast<float>(windowSize.y) - halfH);
        // check if hits an obstacle 




        m_sprite->setPosition(pos);
    }
}

void Player::draw(RenderWindow& window) {
    if (m_loaded && m_sprite) window.draw(*m_sprite);
}