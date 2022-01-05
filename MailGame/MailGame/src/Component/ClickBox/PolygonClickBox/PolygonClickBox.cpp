#include "PolygonClickBox.h"
#include "Game/Game.h"
#include "Entity/Entity.h"
#include <SFML/Graphics/VertexArray.hpp>

PolygonClickBox::PolygonClickBox(Polygon p) : poly(p) {}

bool PolygonClickBox::checkIfClicked(sf::Vector2f mouseCoords) {
	return false;
}

void PolygonClickBox::renderClickBox(sf::RenderWindow* window) {
	auto points = this->poly.getPoints();
	Game* g = this->getEntity()->getGame();
	sf::VertexArray arr(sf::PrimitiveType::LinesStrip, points.size() + 1);
	for (auto it = points.begin(); it != points.end(); it++) {
		arr[it - points.begin()] = sf::Vertex(g->worldToScreenPos(*it), sf::Color::Red);
	}
	arr[points.size()] = sf::Vertex(g->worldToScreenPos(points.front()), sf::Color::Red);
	window->draw(arr);
}