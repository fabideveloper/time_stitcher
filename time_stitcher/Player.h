#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <map>
#include <memory>
#include <vector>

using namespace sf;

class Player {
public:
    // texturePath: imageine
    // startPos: initial position in pixels
    // speed: movement speed in pixels per second
    Player(const std::string& texturePath, const Vector2f& startPos = { 0.f, 0.f }, float speed = 150.f);

    bool isLoaded() const;
    void setPosition(const Vector2f& pos);

    Vector2f getPosition() const;
    FloatRect getBounds() const;

    void update(float dt, const Vector2u& windowSize);
    void draw(RenderWindow& window);

    // Directional sprites API
    enum class Direction {
        Idle,
        Up,
        Down,
        Left,
        Right,
        UpLeft,
        UpRight,
        DownLeft,
        DownRight
    };

    // Legacy: register single textures per direction (keeps compatibility)
    bool setDirectionalTextures(const std::map<Direction, std::string>& paths);

    // New: load frames from a folder structured as:
    // root/
    //   idle/
    //     frame1.png
    //     frame2.png
    //   left/
    //     ...
    // frameTime = seconds per frame
    bool loadDirectionalSpritesFromFolder(const std::string& rootPath, float frameTime = 0.10f);

    void setAnimationFrameTime(float frameTime) { m_frameTime = frameTime; }
    void setAnimateIdle(bool animate) { m_animateIdle = animate; }

private:
    Texture m_texture;
    std::optional<Sprite> m_sprite;
    float m_speed;
    bool m_loaded;

    // Legacy single textures per direction
    std::map<Direction, std::unique_ptr<Texture>> m_textures;

    // Multiple frames per direction (owned)
    std::map<Direction, std::vector<std::unique_ptr<Texture>>> m_frames;

    // animation state
    Direction m_direction = Direction::Idle;
    float m_frameTime = 0.10f; // seconds per frame
    float m_animTimer = 0.f;
    size_t m_frameIndex = 0;
    bool m_animateIdle = false;

    // helper
    Direction chooseDirectionFromRaw(const Vector2f& rawDir) const;
    void applyTextureForDirection(Direction dir); // sets sprite texture + origin
};