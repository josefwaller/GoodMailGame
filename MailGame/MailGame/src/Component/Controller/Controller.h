#pragma once
#include "Component/Component.h"

class Controller : public Component {
public:
	virtual void update(float delta) = 0;
	virtual void onHourChange(size_t newHour) {};
};
