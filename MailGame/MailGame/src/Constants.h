#pragma once
#include <string>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Type declarations
// Game time
typedef unsigned long long gtime_t;
// Ids
typedef unsigned long long id_t;
// The in-game hour
typedef unsigned int hour_t;
// In-game money
typedef long money_t;
// SaveData keys
typedef std::string sdkey_t;

namespace SaveKeys {
	const sdkey_t GAMEMAP = "GameMap";
	const sdkey_t TILE = "tile";
	const sdkey_t X = "x";
	const sdkey_t Y = "y";
	const sdkey_t POSTAL_CODE = "postalCode";
	const sdkey_t TYPE = "type";
	const sdkey_t BUILDING = "building";
	const sdkey_t RAILWAY = "Railway";
	const sdkey_t TO = "to";
	const sdkey_t FROM = "from";
	const sdkey_t IS_STATION = "isStation";
	const sdkey_t ROAD = "Road";
	const sdkey_t HAS_NORTH = "hasNorth";
	const sdkey_t HAS_EAST = "hasEast";
	const sdkey_t HAS_SOUTH = "hasSouth";
	const sdkey_t HAS_WEST = "hasWest";
	const sdkey_t AIRPLANE_ROAD = "AirplaneRoad";
	const sdkey_t IS_RUNWAY = "isRunway";
	const sdkey_t TECH_TREE = "TechTree";
	const sdkey_t GAME = "Game";
	const sdkey_t ENTITIES = "Entities";
	const sdkey_t CAMERA_X = "cameraX";
	const sdkey_t CAMERA_Y = "cameraY";
	const sdkey_t ID = "id";
	const sdkey_t TIME = "time";
	const sdkey_t MONEY = "money";
	const sdkey_t IS_PAUSED = "isPaused";
	const sdkey_t MAIL = "Mail";
	const sdkey_t SOURCE = "src";
	const sdkey_t DEST = "dest";
	const sdkey_t MAIL_TRUCK_ROUTE = "MailTruckRoute";
	const sdkey_t INDEX = "index";
	const sdkey_t IS_DELIVERING = "isDelivering";
	const sdkey_t MAIL_TRUCK_ROUTE_STOP = "MailTruckRouteStop";
	const sdkey_t NUM_STOPS = "numStops";
	const sdkey_t HAS_TARGET = "hasTarget";
	const sdkey_t TARGET_ID = "targetId";
	const sdkey_t ROUTE = "route";
	const sdkey_t VEHICLE_MODEL = "vehicleModel";
	const sdkey_t CARGO_CAR_MODEL = "cargoCarModeL";
	const sdkey_t NUM_CARGO_CARS = "numCargoCars";
	const sdkey_t Z = "z";
	const sdkey_t DEPART_TIME = "departTime";
	const sdkey_t POSITION = "position";
	const sdkey_t EXPECTED_TIME = "expectedTime";
	const sdkey_t ROUTE_POINT = "RoutePoint";
	const sdkey_t TRANSIT_ROUTE = "TransitRoute";
	const sdkey_t TRANSIT_ROUTE_STOP = "TransitRouteStop";
	const sdkey_t PICK_UP = "pickUp";
	const sdkey_t DROP_OFF = "dropOff";
	const sdkey_t IS_UNLOCKED = "isUnlocked";
	const sdkey_t OFFICE_ID = "officeId";
	const sdkey_t DEPOT_ID = "depotId";
	const sdkey_t ROTATION = "rotation";
	const sdkey_t CAR_STOP = "CarStop";
	const sdkey_t TRAIN_STOP = "TrainStop";
	const sdkey_t AIRPLANE_STOP = "AirplaneStop";
	const sdkey_t START = "start";
	const sdkey_t END = "end";
	const sdkey_t DEPOT = "depot";
}
