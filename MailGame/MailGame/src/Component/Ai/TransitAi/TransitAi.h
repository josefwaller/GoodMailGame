#pragma once
#include "Component/Ai/Ai.h"
#include "Component/TransitStop/TransitStop.h"
#include "Routes/TransitRoute/TransitRoute.h"
#include <vector>
#include <memory>

class Entity;

class TransitAi : public Ai {
public:
	TransitAi(TransitRoute route, std::weak_ptr<Entity> office, TransitStop::TransitType type);
	// Overridden methods, see Ai
	virtual void onArriveAtDest() override;
	virtual void onArriveAtStop(size_t stopIndex) override;
	virtual void renderUi() override;
	virtual std::vector<VehicleControllerStop> getStops();
	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData data) override;
private:
	// The depot that the truck started at, and will return to
	std::weak_ptr<Entity> depot;
	// The route the truck is following
	TransitRoute route;
	// The type of stop this vehicle will access
	TransitStop::TransitType type;
};
