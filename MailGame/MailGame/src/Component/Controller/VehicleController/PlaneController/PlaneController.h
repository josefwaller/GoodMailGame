#include "Component/Controller/VehicleController/VehicleController.h"

class PlaneController : public VehicleController {
public:
	enum class State {
		ArrivingAtRunway,
		InDepot,
		Departing,
		InTransit,
		WaitingForLock,
	};
	PlaneController(gtime_t departTime, VehicleModel model);
protected:
	virtual void update(float delta) override;
	virtual void onArriveAtDest(gtime_t arriveTime) override;
private:
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;
	State state;
	// Get the path of a loop above an airport while waiting for the runway to be free
	std::vector<RoutePoint> getLoopPath(RoutePoint start);
};