#include "PolygonClickBox.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include <SFML/Graphics/VertexArray.hpp>

PolygonClickBox::PolygonClickBox(std::vector<sf::Vector2f> p): points(p) {
}

bool PolygonClickBox::checkIfClicked(sf::Vector2f mouseCoords) {
	return false;
}

void PolygonClickBox::renderClickBox(sf::RenderWindow* window) {
	sf::VertexArray arr(sf::PrimitiveType::LineStrip, this->points.size() + 1);
	sf::Color c = this->checkIfClicked(sf::Vector2f(this->getEntity()->getGame()->getWindowMousePosition())) ? sf::Color::Green : sf::Color::Red;
	for (size_t i = 0; i < this->points.size(); i++) {
		arr[i].color = c;
		arr[i].position = this->points.at(i);
	}
	arr[this->points.size()].color = c;
	arr[this->points.size()].position = this->points.front();
	window->draw(arr);
}