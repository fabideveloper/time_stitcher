#include "Game.h"
#include <iostream>

// Helper to create SFML VideoMode
sf::VideoMode CreateVideoMode(unsigned width, unsigned height) {
    sf::VideoMode mode;
    mode.size = sf::Vector2u(width, height);
    mode.bitsPerPixel = 32;
    return mode;
}

Game::Game(unsigned width, unsigned height)
    : window(CreateVideoMode(width, height), "Time Stitcher"),
    player("assets/images/player.png", { width / 2.f, height / 2.f }, 400.f)
{
    if (!backgroundTexture.loadFromFile("assets/images/background.jpg")) {
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
}

void Game::createMaze(sf::Vector2f startPos, sf::Vector2f endPos) {
    // obstacles.emplace_back(sf::Vector2f(200, 200));
    obstacles.clear();
}

void Game::run() {
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