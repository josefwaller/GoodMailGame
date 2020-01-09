#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include <vector>

class PostOfficeController : public Controller {
public:
	virtual void update(float delta) override;
	
	// Update the attributes from the Ui
	void setRouteTime(size_t routeIndex, int departTime);
	void setRouteType(size_t routeIndex, bool isDelivering);
private:
	std::vector<MailTruckRoute> routes;
};
