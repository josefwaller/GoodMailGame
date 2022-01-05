#include "RectClickBox.h"
#include <SFML/Graphics/VertexArray.hpp>
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "Game/Game.h"
#include "System/SaveData/SaveData.h"

RectClickBox::RectClickBox(sf::FloatRect r) : rect(r) {}

bool RectClickBox::checkIfClicked(sf::Vector2f mouseCoords) {
	// Simply check if the coords are in the rect
	sf::FloatRect actualRect = getPositionedRect();
	return actualRect.contains(mouseCoords);
}

sf::FloatRect RectClickBox::getPositionedRect() {
	// ToDo:
	// This is all broken, should really be done better wrt the thrid dimension
	sf::Vector2f pos;
	if (auto trans = this->getEntity()->transform) {
		pos = sf::Vector2f(trans->getPosition().x, trans->getPosition().y);
	}
	return sf::FloatRect(pos.x + rect.left, pos.y + rect.top, rect.width, rect.height);
}

void RectClickBox::renderClickBox(sf::RenderWindow* window) {
	// Get the actual rect
	sf::FloatRect actualRect = getPositionedRect();
	// Transform every point and add them to a vertex array
	Game* g = this->getEntity()->getGame();
	sf::VertexArray v(sf::LineStrip, 5);
	// Add all the points
	v[0].position = g->worldToScreenPos({ actualRect.left, actualRect.top, 0 });
	v[1].position = g->worldToScreenPos({ actualRect.left + actualRect.width, actualRect.top, 0 });
	v[2].position = g->worldToScreenPos({ actualRect.left + actualRect.width, actualRect.top + actualRect.height, 0 });
	v[3].position = g->worldToScreenPos({ actualRect.left, actualRect.top + actualRect.height, 0 });
	v[4].position = g->worldToScreenPos({ actualRect.left, actualRect.top, 0 });
	for (size_t i = 0; i < 5; i++) {
		v[i].color = sf::Color::Red;
	}
	//	window->draw(v);
}