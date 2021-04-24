#pragma once
#include "Component/Controller/Controller.h"
#include "Constants.h"
#include <vector>
#include <SFML/System/Vector2.hpp>

/*
 * The residence controll has a few "friend" residences who it sends letters to every so often
 * 1 from the same city and 2 from a differnet city
 */
class ResidenceController : public Controller {
public:
	ResidenceController(gtime_t currentTime);
	virtual void init() override;
	virtual void onHourChange(hour_t) override;
	virtual std::optional<SaveData> getSaveData() override;
	// Get the desintations
	// Currently used by UiHandler, probably won't make it into final product
	std::vector<sf::Vector2i> getDestinations();
private:
	std::vector<std::pair<hour_t, sf::Vector2i>> destinations;
	void generateLetter(sf::Vector2i dest, gtime_t time);
};
