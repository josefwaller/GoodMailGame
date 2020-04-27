#pragma once
#include "Component/Component.h"
#include "Constants.h"

class Controller : public Component {
public:
	virtual void update(float delta) = 0;
	virtual void onHourChange(hour_t newHour) {};
};
