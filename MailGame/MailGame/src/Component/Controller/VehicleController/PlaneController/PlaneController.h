#include "Component/Controller/VehicleController/VehicleController.h"

class PlaneController : public VehicleController {
public:
	enum class State {
		ArrivingAtRunway,
		InDepot,
		Departing,
		InTransit,
	};
	PlaneController(gtime_t departTime, VehicleModel model);
protected:
	virtual void onArriveAtDest(gtime_t arriveTime) override;
private:
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;
	State state;
};