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
	sf::Vector2f pos;
	if (auto trans = this->getEntity()->transform) {
		pos = trans->getPosition();
	}
	return sf::FloatRect(pos.x + rect.left, pos.y + rect.top, rect.width, rect.height);

}

#ifdef _DEBUG
void RectClickBox::renderClickBox(sf::RenderWindow* window) {
	// Get the actual rect
	sf::FloatRect actualRect = getPositionedRect();
	// Transform every point and add them to a vertex array
	Game* g = this->getEntity()->getGame();
	sf::VertexArray v(sf::LineStrip, 5);
	// Add all the points
	v[0].position = g->worldToScreenPos({ actualRect.left, actualRect.top });
	v[1].position = g->worldToScreenPos({ actualRect.left + actualRect.width, actualRect.top });
	v[2].position = g->worldToScreenPos({ actualRect.left + actualRect.width, actualRect.top + actualRect.height });
	v[3].position = g->worldToScreenPos({ actualRect.left, actualRect.top + actualRect.height });
	v[4].position = g->worldToScreenPos({ actualRect.left, actualRect.top });
	for (size_t i = 0; i < 5; i++) {
		v[i].color = sf::Color::Red;
	}
	window->draw(v);
}
#endif

std::optional<SaveData> RectClickBox::getSaveData() {
	SaveData d("ClickBox");
	d.addValue("x", std::to_string(this->rect.left));
	d.addValue("y", std::to_string(this->rect.top));
	d.addValue("w", std::to_string(this->rect.width));
	d.addValue("h", std::to_string(this->rect.height));
	return d;
}
