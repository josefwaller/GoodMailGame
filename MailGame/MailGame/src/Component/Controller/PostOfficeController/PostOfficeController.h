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
	void deleteRoute(size_t routeIndex);
private:
	// The routes that the post office will spawn trucks for
	std::vector<MailTruckRoute> routes;
	// The routes that should be deleted
	std::vector<size_t> routesToDelete;
};
