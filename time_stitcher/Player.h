#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

using namespace sf;

class Player {
public:
    // texturePath: imageine
    // startPos: initial position in pixels
    // speed: movement speed in pixels per second
    Player(const std::string& texturePath, const Vector2f& startPos = {0.f, 0.f}, float speed = 300.f);

    bool isLoaded() const;
    void setPosition(const Vector2f& pos);

    Vector2f getPosition() const;
    FloatRect getBounds() const;

    void update(float dt, const Vector2u& windowSize);
    void draw(RenderWindow& window);

private:
    Texture m_texture;
    std::optional<Sprite> m_sprite;
    float m_speed;
    bool m_loaded;
};