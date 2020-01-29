#pragma once
#include "Component/Component.h"
#include <vector>
#include <memory>
#include "Mail/Mail.h"

// The component responsible for holding mail
class MailContainer : public Component {
public:
	// Add some mail
	void addMail(std::vector<Mail> mail);
	// Give all the letters to another container
	void transferAllMailTo(std::shared_ptr<MailContainer> other);
private:
	// The mail in the entity right now
	std::vector<Mail> mail;
};
