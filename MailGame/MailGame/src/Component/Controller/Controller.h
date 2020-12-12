#pragma once
#include "Component/Component.h"
#include "Constants.h"

class Controller : public Component {
public:
	virtual void update(float delta) = 0;
	virtual void onHourChange(hour_t newHour) {};
	// Get the cost of this entity, per month
	// Note that vehicles don't cost anything once they are spawned, so the cost is calculated by the office that spawns them
	virtual money_t getCost();
};
