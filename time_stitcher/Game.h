#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Player.h"
#include "Obstacle.h"

class Game {
public:
    Game(unsigned width, unsigned height);
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void createMaze(Vector2f startPos, Vector2f endPos);


    sf::RenderWindow window;
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> background;
    
    Player player;
    std::vector<Obstacle> obstacles;

    sf::Clock clock;
};