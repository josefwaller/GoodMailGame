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
	// Remove some mail
	void removeMail(std::vector<Mail> mail);
	// Give all the letters to another container
	void transferAllMailTo(std::shared_ptr<MailContainer> other);
	// Give the letters given to the container given
	void transferSomeMailTo(std::vector<Mail> toGive, std::shared_ptr<MailContainer> other);
	// Get number of letters
	size_t getNumLetters();
	// Get the mail
	std::vector<Mail> getMail();

	virtual std::optional<SaveData> getSaveData() override;
private:
	// The mail in the entity right now
	std::vector<Mail> mail;
};
