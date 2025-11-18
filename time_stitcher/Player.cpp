#include "Player.h"
#include "Obstacle.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <filesystem> // C++17+
#include <string>

namespace fs = std::filesystem;

Player::Player(const std::string& texturePath, const Vector2f& startPos, float speed)
    : m_speed(speed), m_loaded(false)
{
    if (not m_texture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load " << texturePath << '\n';
        return;
    }

    m_texture.setSmooth(true);
    // construct the sprite only after the texture was successfully loaded
    m_sprite.emplace(m_texture);

    const float desiredPixelSize = 64.f;
	FloatRect localBounds = m_sprite->getLocalBounds();
    if(localBounds.size.x > 0 and localBounds.size.y > 0) {
        float scaleX = desiredPixelSize / localBounds.size.x;
        float scaleY = desiredPixelSize / localBounds.size.y;

		float uniform = std::min(scaleX, scaleY);
        m_sprite->setScale({ uniform, uniform });
	}


    // center origin so rotation/positioning are intuitive
    FloatRect bounds = m_sprite->getLocalBounds();
    m_sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
    // m_sprite.setOrigin({ 0,0 });
    m_sprite->setPosition(startPos);

    m_loaded = true;
}

bool Player::setDirectionalTextures(const std::map<Direction, std::string>& paths) {
    // Try to load each provided texture into m_textures. If any load fails we remove that entry and continue.
    bool anyLoaded = false;
    for (const auto& [dir, path] : paths) {
        auto tex = std::make_unique<Texture>();
        if (not tex->loadFromFile(path)) {
            std::cerr << "Failed to load directional texture: " << path << '\n';
            continue;
        }
        tex->setSmooth(true);
        m_textures[dir] = std::move(tex);
        anyLoaded = true;
    }

    // If directional textures have been provided and the player already has a sprite, update it to a matching texture (Idle -> keep default)
    if (anyLoaded and m_sprite) {
        // if there's a texture for Idle, apply it; otherwise keep the current m_texture
        if (m_textures.count(Direction::Idle)) applyTextureForDirection(Direction::Idle);
    }
    return anyLoaded;
}

// Load frames from folder structure: root/<dir>/*.{png,jpg,...}
bool Player::loadDirectionalSpritesFromFolder(const std::string& rootPath, float frameTime) {
    if (rootPath.empty()) return false;
    fs::path root(rootPath);
    if (not fs::exists(root) or not fs::is_directory(root)) {
        std::cerr << "Player sprite root path does not exist or is not a directory: " << rootPath << '\n';
        return false;
    }

    // helper to map folder name to Direction enum
    auto folderToDirection = [](const std::string& name) -> std::optional<Direction> {
        std::string s = name;
        // normalize: lowercase, replace spaces with underscore
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        if (s == "idle") return Direction::Idle;
        if (s == "up") return Direction::Up;
        if (s == "down") return Direction::Down;
        if (s == "left") return Direction::Left;
        if (s == "right") return Direction::Right;
        if (s == "upleft" or s == "up_left") return Direction::UpLeft;
        if (s == "upright" or s == "up_right") return Direction::UpRight;
        if (s == "downleft" or s == "down_left") return Direction::DownLeft;
        if (s == "downright" or s == "down_right") return Direction::DownRight;
        return std::nullopt;
        };

    bool anyLoaded = false;
    // iterate subdirectories
    for (auto it = fs::directory_iterator(root); it != fs::directory_iterator(); ++it) {
        if (not fs::is_directory(it->path())) continue;
        auto folderName = it->path().filename().string();
        auto maybeDir = folderToDirection(folderName);
        if (not maybeDir) continue;
        Direction dir = *maybeDir;

        // collect files, sort by filename for consistent frame order
        std::vector<fs::path> files;
        for (auto& f : fs::directory_iterator(it->path())) {
            if (fs::is_regular_file(f.path())) {
                auto ext = f.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
                if (ext == ".png" or ext == ".jpg" or ext == ".jpeg" or ext == ".bmp" or ext == ".tga") {
                    files.push_back(f.path());
                }
            }
        }
        if (files.empty()) continue;
        std::sort(files.begin(), files.end());

        auto& vec = m_frames[dir];
        vec.clear();
        for (auto& p : files) {
            auto tex = std::make_unique<Texture>();
            if (not tex->loadFromFile(p.string())) {
                std::cerr << "Failed to load frame: " << p.string() << '\n';
                continue;
            }
            tex->setSmooth(true);
            vec.push_back(std::move(tex));
        }
        if (not vec.empty()) {
            anyLoaded = true;
        }
        else {
            // remove empty entry
            m_frames.erase(dir);
        }
    }

    if (anyLoaded) {
        m_frameTime = frameTime;
        m_animTimer = 0.f;
        m_frameIndex = 0;
        // apply initial texture for current direction (or idle)
        if (m_sprite) {
            applyTextureForDirection(m_direction);
        }
    }

    return anyLoaded;
}

Vector2f Player::getPosition() const {
    if (m_sprite) return m_sprite->getPosition();
    return { 0.f, 0.f };
}

FloatRect Player::getBounds() const {
    if (m_sprite) return m_sprite->getGlobalBounds();
    // fallback: empty rect at current position
    Vector2f p = getPosition();
    return FloatRect({ p.x, p.y }, { 0.f, 0.f });
}

bool Player::isLoaded() const {
    return m_loaded;
}

void Player::setPosition(const Vector2f& pos) {
    if (m_sprite) m_sprite->setPosition(pos);
}

Player::Direction Player::chooseDirectionFromRaw(const Vector2f& rawDir) const {
    // rawDir contains -1/0/1 values for axis (before normalization)
    int sx = (rawDir.x > 0.0f) ? 1 : (rawDir.x < 0.0f ? -1 : 0);
    int sy = (rawDir.y > 0.0f) ? 1 : (rawDir.y < 0.0f ? -1 : 0);

    if (sx == 0 and sy == 0) return Direction::Idle;
    if (sx == 0 and sy < 0) return Direction::Up;
    if (sx == 0 and sy > 0) return Direction::Down;
    if (sx < 0 and sy == 0) return Direction::Left;
    if (sx > 0 and sy == 0) return Direction::Right;
    if (sx < 0 and sy < 0) return Direction::UpLeft;
    if (sx > 0 and sy < 0) return Direction::UpRight;
    if (sx < 0 and sy > 0) return Direction::DownLeft;
    if (sx > 0 and sy > 0) return Direction::DownRight;
    return Direction::Idle;
}

void Player::applyTextureForDirection(Direction dir) {
    // Prefer animated frames if available
    if (not m_sprite) return;

    auto fit = m_frames.find(dir);
    if (fit != m_frames.end() and not fit->second.empty()) {
        // clamp index
        if (m_frameIndex >= fit->second.size()) m_frameIndex = 0;
        m_sprite->setTexture(*fit->second[m_frameIndex]);
    }
    else {
        // fallback to single texture per direction (legacy)
        auto it = m_textures.find(dir);
        if (it != m_textures.end() and it->second) {
            m_sprite->setTexture(*it->second);
        }
        else {
            // last fallback: original m_texture
            m_sprite->setTexture(m_texture);
        }
    }

    // Re-center origin because texture size may differ between directions/frames
    FloatRect bounds = m_sprite->getLocalBounds();
    m_sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
}

void Player::update(float dt, const Vector2u& windowSize) {
    if (not m_loaded) return;

    Vector2f rawDir(0.f, 0.f);
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) rawDir.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) rawDir.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) rawDir.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) rawDir.x += 1.f;

    Vector2f dir = rawDir;
    bool moving = (dir.x != 0.f or dir.y != 0.f);

    if (moving) {
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

    // choose direction from raw (pre-normalized) input so diagonals map correctly
    Direction newDir = chooseDirectionFromRaw(rawDir);
    if (newDir != m_direction) {
        m_direction = newDir;
        m_frameIndex = 0;     // restart animation on direction change
        m_animTimer = 0.f;
        applyTextureForDirection(m_direction);
    }

    // advance animation frames if we have frames for this direction
    bool shouldAnimate = moving or m_animateIdle;
    auto fit = m_frames.find(m_direction);
    if (fit != m_frames.end() and not fit->second.empty() and shouldAnimate) {
        m_animTimer += dt;
        if (m_animTimer >= m_frameTime) {
            m_animTimer -= m_frameTime;
            m_frameIndex = (m_frameIndex + 1) % fit->second.size();
            // update sprite to new frame
            m_sprite->setTexture(*fit->second[m_frameIndex]);
            // re-center origin
            FloatRect bounds = m_sprite->getLocalBounds();
            m_sprite->setOrigin({ bounds.size.x * 0.5f, bounds.size.y * 0.5f });
        }
    }
    else {
        // If no frames for this direction, but we have a single texture, ensure it's applied
        if (m_textures.count(m_direction) and m_sprite) {
            m_sprite->setTexture(*m_textures[m_direction]);
        }
    }
}

void Player::draw(RenderWindow& window) {
    if (m_loaded and m_sprite) window.draw(*m_sprite);
}