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
			// Destroy the building there if there is one
			// ToDo: Figure out how this would work if the building is more than one square in size
			std::weak_ptr<Entity> building = g->getGameMap()->getTileAt(tilesPos.x, tilesPos.y).building;
			if (building.lock()) {
				g->removeEntity(building);
			}
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
			switch (rot.getRotation() + g->getRotation().getRotation()) {
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
	}},
	{ EntityTag::MailBox, {
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			sf::Vector2i tilePos((int)floor(pos.x), (int)floor(pos.y));
			// Add the mailbox
			std::shared_ptr<Entity> mailBox = EntityPresets::mailBox(g, sf::Vector2f(tilePos), rot);
			// Set as building on the road
			g->getGameMap()->getTileAt(tilePos.x, tilePos.y).building = mailBox;
			return mailBox;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			// Just check if the tile is a road and doesn't have a building
			Tile t = g->getGameMap()->getTileAt((int)floor(pos.x), (int)floor(pos.y));
			if (t.building.lock())
				return false;
			else if (t.type != TileType::Road)
				return false;
			else
				return true;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot, bool isValid) {
			sf::Sprite s = ResourceLoader::get()->getSprite("buildings/buildings", "mailbox");
			if (isValid)
				s.setColor(sf::Color::Green);
			else
				s.setColor(sf::Color::Red);
			s.setPosition(g->worldToScreenPos({ floor(pos.x), floor(pos.y) }));
			// Set origin to bottom center
			s.setOrigin(s.getLocalBounds().width / 2, s.getLocalBounds().height);
			return s;
		}
	} }
};

