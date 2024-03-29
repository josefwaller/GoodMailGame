#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include "Mail/Mail.h"
#include <vector>
#include <optional>

class PostOfficeController : public Controller {
public:
	virtual void update(float delta) override;
	virtual void onHourChange(hour_t newHour) override;
	virtual void renderUi() override;
	// Get the cost of a given route
	money_t getRouteCost(MailTruckRoute r);
	// Get the cost of the entire post office
	virtual money_t getCost() override;

	// Update the attributes from the Ui
	void addStop(size_t routeIndex, MailTruckRouteStop stop);
	void setStopTile(size_t routeIndex, size_t stopIndex, sf::Vector2i tilePos);
	void deleteStop(size_t routeIndex, size_t stopIndex);
	void setRouteTime(size_t routeIndex, gtime_t departTime);
	void setRouteType(size_t routeIndex, bool isDelivering);
	void addRoute(MailTruckRoute r);
	void deleteRoute(size_t routeIndex);

	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData data) override;
private:
	// The routes that the post office will spawn trucks for
	std::vector<MailTruckRoute> routes;
	// The routes that should be deleted
	std::vector<size_t> routesToDelete;
	// The postal code this postal office manages
	// TODO eventually, these postal codes won't show up in the UI since the player is not allowed to change them
	std::optional<id_t> postalCodeId;
	// The time of day that the postoffice last checked for spawning a truck
	gtime_t lastTruckSpawnTime;
	MailTruckRoute prepareRouteForTruck(MailTruckRoute route);
	// Reset the postal code to all the tiles that this post office delivers to
	void resetPostalCode();
	// Get length of the given route in tiles
	size_t getRouteLength(MailTruckRoute r);
};
