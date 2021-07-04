#include "Component/Controller/VehicleController/VehicleController.h"

class BoatController : public VehicleController {
public:
	BoatController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars);
	virtual void init() override;
	virtual void onArriveAtDest(gtime_t arriveTime) override;

	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData) override;
private:
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;
};