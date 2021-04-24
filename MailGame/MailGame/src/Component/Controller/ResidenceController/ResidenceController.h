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
	virtual void update(float delta) override;
	static const gtime_t GENERATE_INTERVAL;
	virtual std::optional<SaveData> getSaveData() override;
	// Get the desintations
	// Currently used by UiHandler, probably won't make it into final product
	std::vector<sf::Vector2i> getDestinations();
private:
	std::vector<sf::Vector2i> destinations;
	gtime_t lastGenTime;
	void generateLetter();
};
