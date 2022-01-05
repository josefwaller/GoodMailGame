#include "PolygonClickBox.h"
#include "Game/Game.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include <SFML/Graphics/VertexArray.hpp>

PolygonClickBox::PolygonClickBox(Polygon p, bool r) : poly(p), relativeToCenter(r) {}

bool PolygonClickBox::checkIfClicked(sf::Vector2f mouseCoords) {
	mouseCoords -= this->getOffset();
	std::vector<sf::Vector2f> screenPoints = this->poly.getPoints();
	// Used 2D ray casting
	size_t numHits = 0;
	for (size_t i = 0; i < screenPoints.size(); i++) {
		sf::Vector2f pOne = screenPoints.at(i);
		sf::Vector2f pTwo = screenPoints.at((i + 1) % screenPoints.size());
		// One must be above and one must be below
		if ((pOne.y > mouseCoords.y && pTwo.y <= mouseCoords.y) || (pOne.y <= mouseCoords.y && pTwo.y > mouseCoords.y)) {
			// Edge case: The line is vertical
			// quickly check for that
			if (pOne.x == pTwo.x) {
				if (pOne.x > mouseCoords.x) {
					numHits++;
				}
				continue;
			}
			// Check that the point's collision with the line is inbetween thw two points
			float m = (pOne.y - pTwo.y) / (pOne.x - pTwo.x);
			float b = pOne.y - m * pOne.x;

			float x = (mouseCoords.y - b) / m;

			float minX = std::fminf(pOne.x, pTwo.x);
			float maxX = minX + fabsf(pOne.x - pTwo.x);
			if (x > minX && x < maxX) {
				if (x > mouseCoords.x) {
					numHits++;
				}
			}
		}
	}
	return numHits % 2 == 1;
}

void PolygonClickBox::renderClickBox(sf::RenderWindow* window) {
	auto points = this->poly.getPoints();
	Game* g = this->getEntity()->getGame();
	sf::Vector2f offset = this->getOffset();
	sf::VertexArray arr(sf::PrimitiveType::LinesStrip, points.size() + 1);
	sf::Color c = this->checkIfClicked(sf::Vector2f(g->getWindowMousePosition())) ? sf::Color::Green : sf::Color::Red;
	for (auto it = points.begin(); it != points.end(); it++) {
		arr[it - points.begin()] = sf::Vertex(*it + offset, c);
	}
	arr[points.size()] = sf::Vertex(points.front() + offset, c);
	window->draw(arr);
}

sf::Vector2f PolygonClickBox::getOffset() {
	sf::Vector3f offset = this->getEntity()->transform->getPosition();
	if (this->relativeToCenter) {
		offset += sf::Vector3f(0.5, 0.5, 0);
	}
	return this->getEntity()->getGame()->worldToScreenPos(offset);
}