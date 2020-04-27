#pragma once
#include "Component/Controller/Controller.h"
#include "Constants.h"

class ResidenceController : public Controller {
public:
	ResidenceController(gtime_t currentTime);
	virtual void update(float delta) override;
	static const gtime_t GENERATE_INTERVAL;
	virtual std::optional<SaveData> getSaveData() override;
private:
	gtime_t lastGenTime;
	void generateLetter();
};
