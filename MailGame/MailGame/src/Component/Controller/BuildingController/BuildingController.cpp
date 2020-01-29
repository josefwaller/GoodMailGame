#include "BuildingController.h"
#include "Entity/Entity.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/Transform/Transform.h"
#include "Mail/Letter/Letter.h"
#include <imgui.h>

const float BuildingController::GENERATE_INTERVAL = 5000.0f;

void BuildingController::update(float time) {
	if (clk.getElapsedTime().asMilliseconds() >= GENERATE_INTERVAL) {
		clk.restart();
		this->generateLetter();
	}
	// List how much mail is in this building
	ImGui::Begin("Building Mail");
	char buf[200];
	auto trans = this->getEntity()->transform;
	auto mail = this->getEntity()->mailContainer;
	sprintf_s(buf, "B at (%d, %d): %zu letters",
		(int)(trans->getPosition().x),
		(int)(trans->getPosition().y),
		mail->getNumLetters());
	ImGui::Text(buf);
	ImGui::End();
}

void BuildingController::generateLetter() {
	sf::Vector2i pos(this->getEntity()->transform->getPosition());
	this->getEntity()->mailContainer->addMail({ Letter(pos, pos) });
}
