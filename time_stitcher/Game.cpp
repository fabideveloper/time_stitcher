#include "Game.h"
#include <iostream>

// Test
// Helper to create SFML VideoMode
sf::VideoMode CreateVideoMode(unsigned width, unsigned height) {
    sf::VideoMode mode;
    mode.size = sf::Vector2u(width, height);
    mode.bitsPerPixel = 32;
    return mode;
}

Game::Game(unsigned width, unsigned height)
    : window(CreateVideoMode(width, height), "Time Stitcher"),
    player("assets/images/player_sprites/player.png", { width / 2.f, height / 2.f }, 400.f)
{
    if (not backgroundTexture.loadFromFile("assets/images/background.jpg")) {
        std::cerr << "Failed to load background\n";
    }
    else {
        // construct the sprite only after the texture is loaded
        background.emplace(backgroundTexture);
        auto texSize = backgroundTexture.getSize();
        if (texSize.x > 0) {
            background->setScale(
                { static_cast<float>(width) / texSize.x,
                  static_cast<float>(height) / texSize.y }
            );
        }
    }

    /*player.setDirectionalTextures({
       { Player::Direction::Idle,      "assets/images/player_idle.png" },
       { Player::Direction::Left,      "assets/images/player_left.png" },
       { Player::Direction::Right,     "assets/images/player_right.png" },
       { Player::Direction::Up,        "assets/images/player_up.png" },
       { Player::Direction::Down,      "assets/images/player_down.png" },
       { Player::Direction::UpLeft,    "assets/images/player_up_left.png" },
       { Player::Direction::UpRight,   "assets/images/player_up_right.png" },
       { Player::Direction::DownLeft,  "assets/images/player_down_left.png" },
       { Player::Direction::DownRight, "assets/images/player_down_right.png" }
        });*/

        // load animations from folder structure "assets/player_sprites/<direction>/*"
    if (!player.loadDirectionalSpritesFromFolder("assets/images/player_sprites", 0.1f)) {
        std::cerr << "Warning: no directional sprite folders found, falling back to single textures\n";
    }
    // optional: animate idle frames too
    player.setAnimateIdle(true);
}

void Game::createMaze(sf::Vector2f startPos, sf::Vector2f endPos) {
    // obstacles.emplace_back(sf::Vector2f(200, 200));
    obstacles.clear();

}

void Game::run() {
    this->createMaze({ 0.f,0.f }, { 0.f,0.f });
    while (window.isOpen()) {
        processEvents();
        render();
        update(clock.restart().asSeconds());
    }
    
}

void Game::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void Game::update(float dt) {
    sf::Vector2f prevPos = player.getPosition();
    player.update(dt, window.getSize());

    for (auto& obs : obstacles) {
        if (obs.intersects(player.getBounds())) {
            obs.touched();
            player.setPosition(prevPos);
        }
        obs.update(dt);
    }
}

void Game::render() {
    window.clear();
    if (background) window.draw(*background);
    player.draw(window);

    for (auto& obs : obstacles)
        obs.draw(window);

    window.display();
}