#include <iostream>
#include <chrono>
#include "SFML/Graphics.hpp"

const int FRAMERATE = 60;
constexpr unsigned int FRAME_DURATION = int(1000000 / FRAMERATE);

int main() {
	unsigned lag = 0;

	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	previous_time = std::chrono::steady_clock::now();

	sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	window.setKeyRepeatEnabled(true);
	//Main Loop
	while (window.isOpen()) {



		//The difference in time between the current frame and the previous frame
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		//Add the difference to the lag
		lag += delta_time;

		//Updating the current time for the next frame.
		previous_time += std::chrono::microseconds(delta_time);

		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;
			while (const std::optional event = window.pollEvent()) {
				if (event->is<sf::Event::Closed>()) {
					window.close();
				}
			}
			if (FRAME_DURATION > lag) {
				window.clear();
				window.draw(shape);
				window.display();
			}
		}
	}
	return 0;
}