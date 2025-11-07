#include <SFML/Graphics.hpp>
#include "time.h"

#define WIDTH 800
#define HEIGHT 600
#define FPS 60

using namespace sf;

int main() {

    VideoMode videoMode;
    videoMode.size = sf::Vector2u(WIDTH, HEIGHT);
    videoMode.bitsPerPixel = 64;
    RenderWindow window(videoMode, "Time Stitcher");

    bool drawCircle = true;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        window.clear(Color::Black);

        if (drawCircle) {
            RectangleShape shape({ 500.f, 100.f });
            shape.setFillColor(Color::Green);
            window.draw(shape);
        }

        drawCircle = not drawCircle;


        window.display();

        sleep(milliseconds(FPS / 10));
    }

    return 0;
}