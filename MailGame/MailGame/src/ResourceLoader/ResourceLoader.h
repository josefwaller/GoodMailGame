#pragma once
#include <map>
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Rect.hpp"

struct Spritesheet {
	// The texture of the spritesheet
	sf::Texture texture;
	// The sprites on the sheet, by name
	std::map<std::string, sf::Rect<unsigned int>> sprites;
};
/*
 * Singleton which is responsible for only loading every resource once
 */
class ResourceLoader {
private:
	ResourceLoader();
	std::map<std::string, Spritesheet> spritesheets;
	void ensureSpritesheetLoaded(std::string sheetName);
public:
	static ResourceLoader get();
	sf::Sprite getSprite(std::string sheetName, std::string spriteName);
};
