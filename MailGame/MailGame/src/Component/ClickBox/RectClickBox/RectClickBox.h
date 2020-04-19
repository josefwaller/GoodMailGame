#pragma once
#include "Component/ClickBox/ClickBox.h"
#include <SFML/Graphics/Rect.hpp>

class RectClickBox : public ClickBox {
public:
	RectClickBox(sf::FloatRect rect);
	// Check if the entity is clicked, with given coords in GAME POSITION
	virtual bool checkIfClicked(sf::Vector2f mouseCoords) override;
#ifdef _DEBUG
	virtual void renderClickBox(sf::RenderWindow* w) override;
#endif
	virtual SaveData getSaveData() override;
private:
	sf::FloatRect rect;
	// Get the rect after it has been moved to the entity's position, etc
	sf::FloatRect getPositionedRect();
};
