#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include <vector>

class PostOfficeController : public Controller {
public:
	virtual void update(float delta) override;

private:
	std::vector<MailTruckRoute> routes;
};
