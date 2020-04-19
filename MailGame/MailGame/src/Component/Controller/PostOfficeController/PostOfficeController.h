#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include "Mail/Mail.h"
#include <vector>

class PostOfficeController : public Controller {
public:
	virtual void update(float delta) override;
	virtual void onHourChange(size_t newHour) override;

	// Update the attributes from the Ui
	void addStop(size_t routeIndex, MailTruckRouteStop stop);
	void setStopTile(size_t routeIndex, size_t stopIndex, sf::Vector2i tilePos);
	void deleteStop(size_t routeIndex, size_t stopIndex);
	void setRouteTime(size_t routeIndex, int departTime);
	void setRouteType(size_t routeIndex, bool isDelivering);
	void addRoute(MailTruckRoute r);
	void deleteRoute(size_t routeIndex);

	virtual std::optional<SaveData> getSaveData() override;
private:
	// The routes that the post office will spawn trucks for
	std::vector<MailTruckRoute> routes;
	// The routes that should be deleted
	std::vector<size_t> routesToDelete;
};
