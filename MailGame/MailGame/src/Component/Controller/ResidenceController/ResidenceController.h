#pragma once
#include "Component/Controller/Controller.h"
#include <SFML/System/Clock.hpp>

class ResidenceController : public Controller {
public:
	virtual void update(float delta) override;
	const static float GENERATE_INTERVAL;
	virtual std::optional<SaveData> getSaveData() override;
private:
	sf::Clock clk;
	void generateLetter();
};
