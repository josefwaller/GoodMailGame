#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/TransitRoute/TransitRoute.h"
#include <memory>

class Entity;

class TrainController : public Controller {
public:
	TrainController(TransitRoute route, std::weak_ptr<Entity> depot);
	virtual void update(float) override;
private:
	// The route the train is following
	TransitRoute route;
	// The depot the train spawned from/will return to
	std::weak_ptr<Entity> depot;
};
