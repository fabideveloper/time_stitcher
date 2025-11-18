//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include "time.h"
//#include "Player.h"
//#include "Obstacle.h"
//
//#define WIDTH 800
//#define HEIGHT 600
//
//using namespace sf;
//
//void drawScreen(RenderWindow& window) {
//    // Placeholder for future screen drawing logic
//
//}
//
//void renderStepped(RenderWindow& window) {
//    // Placeholder for future stepped rendering logic
//
//}
//
//int main() {
//
//    VideoMode videoMode;
//    videoMode.size = sf::Vector2u(WIDTH, HEIGHT);
//    videoMode.bitsPerPixel = 64;
//    RenderWindow window(videoMode, "Time Stitcher");
//
//
//    Texture backgroundTexture;
//    if (!backgroundTexture.loadFromFile("assets/images/background.jpg")) {
//        std::cerr << "Failed to load assets/images/background.jpg\n" << __LINE__ << " in main.";
//        return 1;
//    }
//    Sprite background(backgroundTexture);
//
//    Vector2u texSize = backgroundTexture.getSize();
//    if (texSize.x > 0 && texSize.y > 0) {
//        float scaleX = static_cast<float>(WIDTH) / texSize.x;
//        float scaleY = static_cast<float>(HEIGHT) / texSize.y;
//        background.setScale({ scaleX, scaleY });
//    }
//
//
//
//    //  plr centered in the window,
//    Player player("assets/images/player.png", { WIDTH / 2.f, HEIGHT / 2.f }, 400.f);
//    if (!player.isLoaded()) {
//        std::cerr << "Player texture failed to load. Make sure assets/images/player.png exists.\n" << __LINE__ << " in main.";
//        return 1;
//    }
//
//
//    // obstacles
//    Obstacle obstacle_1(Vector2f(200.f, 200.f)); //, "assets/images/obstacle_32x32.png");
//
//    std::vector<Obstacle> obstacles = {obstacle_1};
// 
//    Clock clock;
//    while (window.isOpen()) {
//        // inchid sau nu
//        while (auto event = window.pollEvent()) {
//            if (event->is<Event::Closed>()) {
//                window.close();
//            }
//        }
//        window.clear(Color::Black);
//
//        float dt = clock.restart().asSeconds();
//        Vector2f prevPos = player.getPosition();
//
//        // Update player with delta time
//        
//        player.update(dt, window.getSize());
//        for (auto& obs : obstacles) {
//            if (obs.intersects(player.getBounds())) {
//                obs.touched();
//                obs.update(dt);
//                player.setPosition(prevPos);  // simple resolution: undo the last movement
//                break;
//            }
//        }
//
//       
//        window.draw(background);
//        player.draw(window);
//
//        for (auto& obs : obstacles) {
//            obs.draw(window);
//        }
//
//        window.display();
//    }
//
//    return 0;
//}

#include "Game.h"

int main() {
    Game game(800, 600);
    game.run();
    return 0;
}
