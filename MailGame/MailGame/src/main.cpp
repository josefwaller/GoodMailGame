#include <SFML/Graphics.hpp>

int main() {
	sf::RenderWindow window(sf::VideoMode(200, 200), "Test");
	while (window.isOpen()) {
		sf::Event e;
		while(window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
		window.clear();
		window.display();
	}
	return 0;
}