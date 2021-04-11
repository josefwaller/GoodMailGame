#pragma once
#include "Component/Component.h"
#include "Constants.h"

class Controller : public Component {
public:
	// Initialization method
	// Called when it is safe to initialize, since when loading we need to construct entities without initializing them
	// Currently not called when loading, usually they are initialized in fromSaveData
	virtual void init() {};
	// Update method
	virtual void update(float delta) {};
	virtual void onHourChange(hour_t newHour) {};
	virtual void renderUi() {};
	// Get the cost of this entity, per month
	// Note that vehicles don't cost anything once they are spawned, so the cost is calculated by the office that spawns them
	virtual money_t getCost();
};
