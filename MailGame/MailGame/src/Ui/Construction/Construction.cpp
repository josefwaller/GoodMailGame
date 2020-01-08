#include "Construction.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include "Game/Game.h"
#include <math.h>
#include "ResourceLoader/ResourceLoader.h"
#include "System/Utils/Utils.h"

std::map<EntityTag, Construction::Recipe> Construction::recipes = {
	{ EntityTag::PostOffice, {
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			sf::Vector2i tilesPos((int)round(pos.x), (int)round(pos.y));
			std::shared_ptr<Entity> e = EntityPresets::postOffice(g, sf::Vector2f(tilesPos), rot);
			g->getGameMap()->setBuildingForTile(tilesPos.x, tilesPos.y, e);
			return e;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			// Check the post office is not on a road
			sf::Vector2i intPos((int)round(pos.x), (int)round(pos.y));
			GameMap* gMap = g->getGameMap();
			TileType tile = gMap->getTileAt(intPos.x, intPos.y).type;
			if (tile == TileType::Road || tile == TileType::OffMap) {
				return false;
			}

			// Check it is connected to a road
			size_t x = intPos.x;
			size_t y = intPos.y;
			switch (rot.getRotation()) {
			case IsoRotation::NORTH:
				y -= 1;
				break;
			case IsoRotation::SOUTH:
				y += 1;
				break;
			case IsoRotation::EAST:
				x += 1;
				break;
			default:
				x -= 1;
				break;
			}
			if (gMap->getTileAt(x, y).type != TileType::Road) {
				return false;
			}

			return true;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot, bool isValid) {
			std::string spriteName;
			switch (rot.getRotation()) {
			case IsoRotation::NORTH:
				spriteName = "postOffice-N";
				break;
			case IsoRotation::SOUTH:
				spriteName = "postOffice-S";
				break;
			case IsoRotation::EAST:
				spriteName = "postOffice-E";
				break;
			default:
				spriteName = "postOffice-W";
				break;
			}
			pos = sf::Vector2f(round(pos.x), round(pos.y));
			sf::Sprite toReturn = ResourceLoader::get()->getSprite("buildings/buildings", spriteName);
			toReturn = Utils::setupBuildingSprite(toReturn);
			// Make sprite red if it is invalid
			if (isValid) {
				toReturn.setColor(sf::Color::Green);
			}
			else {
				toReturn.setColor(sf::Color::Red);
			}
			pos = g->worldToScreenPos(pos);
			toReturn.setPosition(pos);
			return toReturn;
		}
	}}
};

