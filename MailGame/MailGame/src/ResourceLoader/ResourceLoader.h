#pragma once
#include <map>
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Font.hpp"

struct Spritesheet {
	// The texture of the spritesheet
	sf::Texture texture;
	// The sprites on the sheet, by name
	std::map<std::string, sf::Rect<int>> sprites;
};
/*
 * Singleton which is responsible for only loading every resource once
 */
class ResourceLoader {
private:
	// Map of spritesheet names to spritesheets
	std::map<std::string, Spritesheet> spritesheets;
	// Mpa of fonts loaded
	std::map<std::string, sf::Font> fonts;
	// Loads the spritesheet if not already loaded
	void ensureSpritesheetIsLoaded(std::string sheetName);
public:
	static ResourceLoader* get();
	// Gets a single sprite from a spritesheet, loading the sheet if neccessary
	// Optional parameter to center it
	sf::Sprite getSprite(std::string sheetName, std::string spriteName, bool center = false);
	// Get a single sprite fro its own image
	sf::Sprite getIndividualSprite(std::string spriteName, bool center = false);
	// Get a font
	sf::Font getFont(std::string fontFile);
};
