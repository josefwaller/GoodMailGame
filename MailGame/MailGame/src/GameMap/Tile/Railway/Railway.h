#pragma once
#include "System/IsoRotation/IsoRotation.h"
#include <vector>

class SaveData;

// The railway on a tile
class Railway {
public:
	// Whether the railway is a station or not
	bool isStation;
	Railway(IsoRotation from, IsoRotation to, bool isStation = false) : directions({ { from, to } }), isStation(isStation) {}
	Railway(SaveData data);
	/**
	 *Get the connected tiles from the ingoing direction provided
	 * ingoingDirection is the direction the train/etc is coming from, relative to the tile
	 * i.e. if the ingoinDirection is south, the train is coming from the tile below this one
	 */
	std::vector<IsoRotation> getOutgoingDirections(IsoRotation ingoingDirection);
	// Add a direction
	void addDirection(IsoRotation from, IsoRotation to);
	// Used for rendering
	std::vector<std::pair<IsoRotation, IsoRotation>> getDirections();
private:
	// The directions the Railway goes in
	// i.e. { North, South } means that the railway goes from the north to the south
	std::vector<std::pair<IsoRotation, IsoRotation>> directions;
};
