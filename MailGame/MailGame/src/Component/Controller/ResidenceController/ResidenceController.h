#pragma once
#include "Component/Controller/Controller.h"
#include <SFML/System/Clock.hpp>

class ResidenceController : public Controller {
public:
	virtual void update(float delta) override;
	const static float GENERATE_INTERVAL;
private:
	sf::Clock clk;
	void generateLetter();
};
