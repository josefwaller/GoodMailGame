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
	void transferAllMailTo(std::shared_ptr<MailContainer> other, MailEvent e);
	void transferAllMailTo(std::shared_ptr<MailContainer> other, std::optional<MailEvent> e = {});
	// Give the letters given to the container given
	// Dumb interface to allow for an optional event to be passed down
	void transferSomeMailTo(std::vector<Mail> toGive, std::shared_ptr<MailContainer> other, MailEvent e);
	void transferSomeMailTo(std::vector<Mail> toGive, std::shared_ptr<MailContainer> other, std::optional<MailEvent> e = {});
	// Remove mail that has been deleted/expired
	void updateMail();
	// Get number of letters
	size_t getNumLetters();
	// Get the mail
	std::vector<Mail> getMail();

	virtual std::optional<SaveData> getSaveData() override;
	void fromSaveData(SaveData data) override;
private:
	// The mail in the entity right now
	std::vector<Mail> mail;
};
