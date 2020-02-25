#include "TrainController.h"

TrainController::TrainController(TransitRoute r, std::weak_ptr<Entity> d) : route(r), depot(d) {}

void TrainController::update(float delta) {

}
