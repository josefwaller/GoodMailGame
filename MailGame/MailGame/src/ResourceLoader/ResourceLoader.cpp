#include "ResourceLoader\ResourceLoader.h"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <stdlib.h>

ResourceLoader* ResourceLoader::get() {
	static ResourceLoader inst;
	return &inst;
}

void ResourceLoader::ensureSpritesheetIsLoaded(std::string sheetName) {
	const std::string PREFIX = "assets/sprites/";
	// Load the spritesheet if it's not already loaded
	if (this->spritesheets.find(sheetName) == this->spritesheets.end()) {
		// Get the image path
		std::string imgPath = PREFIX + sheetName + ".png";
		// Create a new spritesheet and load the texture
		this->spritesheets[sheetName] = Spritesheet();
		sf::Texture* texture = &this->spritesheets[sheetName].texture;
		if (!texture->loadFromFile(imgPath)) {
			// TBA
		}
		// Load the IntRects for every sprite in the spritesheet
		rapidxml::file<> srcFile(std::string(PREFIX + sheetName + ".xml").c_str());
		rapidxml::xml_document<> doc;
		doc.parse<0>(srcFile.data());
		// We assume that the texture atlas is just a single "TextureAtlas" node with
		// a "SubTexture" node for every sprite, which has "name", "x", "y", "width"
		// and "height" attributes
		rapidxml::xml_node<>* textureAtlas = doc.first_node("TextureAtlas");

		for (rapidxml::xml_node<>* subTexture = textureAtlas->first_node("SubTexture"); subTexture != nullptr; subTexture = subTexture->next_sibling()) {
			// Get attributes
			std::string name = subTexture->first_attribute("name")->value();
			int x = std::stoi(subTexture->first_attribute("x")->value());
			int y = std::stoi(subTexture->first_attribute("y")->value());
			int w = std::stoi(subTexture->first_attribute("width")->value());
			int h = std::stoi(subTexture->first_attribute("height")->value());
			// Create IntRect
			sf::Rect<int> spriteRect(x, y, w, h);
			this->spritesheets[sheetName].sprites[name] = spriteRect;
		}
	}
}
sf::Sprite ResourceLoader::getSprite(std::string sheetName, std::string spriteName, bool center) {
	// Load the sheet if neccessary
	this->ensureSpritesheetIsLoaded(sheetName);
	Spritesheet* s = &this->spritesheets[sheetName];
	// Get the sprite's rect
	sf::Rect<int> spriteRect = s->sprites[spriteName];
	// Build the sprite object
	sf::Sprite spr(s->texture, spriteRect);
	if (center) {
		spr.setOrigin(spr.getLocalBounds().width / 2.0f, spr.getLocalBounds().height / 2.0f);
	}
	return spr;
}

sf::Sprite ResourceLoader::getIndividualSprite(std::string spriteName, bool center) {
	// Check if we already have the file loaded
	if (this->spritesheets.find(spriteName) == this->spritesheets.end()) {
		sf::Texture* t = &this->spritesheets[spriteName].texture;
		if (!t->loadFromFile("assets/sprites/" + spriteName)) {
			// TBA
		}
	}
	sf::Sprite spr(this->spritesheets[spriteName].texture);
	if (center) {
		spr.setOrigin(spr.getLocalBounds().width / 2.0f, spr.getLocalBounds().height / 2.0f);
	}
	return spr;
}

sf::Font ResourceLoader::getFont(std::string fontFile) {
	if (this->fonts.find(fontFile) == this->fonts.end()) {
		sf::Font f;
		if (!f.loadFromFile("assets/fonts/" + fontFile)) {
			throw std::runtime_error("Font file not found!");
		}
		this->fonts.insert({ fontFile, f });
	}
	return this->fonts.at(fontFile);
}