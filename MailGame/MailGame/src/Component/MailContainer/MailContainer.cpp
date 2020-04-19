#include "MailContainer.h"
#include "System/SaveData/SaveData.h"
#include <algorithm>
#include <stdexcept>

void MailContainer::addMail(std::vector<Mail> toAdd) {
	this->mail.insert(this->mail.end(), toAdd.begin(), toAdd.end());
}
void MailContainer::transferAllMailTo(std::shared_ptr<MailContainer> other) {
	other->addMail(this->mail);
	this->mail = {};
}
void MailContainer::transferSomeMailTo(std::vector<Mail> toGive, std::shared_ptr<MailContainer> other) {
#ifdef _DEBUG
	// Assert that we are not giving away mail we don't have
	for (Mail m : toGive) {
		if (std::find(this->mail.begin(), this->mail.end(), m) == this->mail.end()) {
			throw std::runtime_error("Tried to give away mail that the MailContainer didn't have!");
		}
	}
#endif
	// Remove the mail
	this->removeMail(toGive);
	// Add the mail to the other
	other->addMail(toGive);
}
size_t MailContainer::getNumLetters() {
	return this->mail.size();
}
void MailContainer::removeMail(std::vector<Mail> mList) {
	for (auto m : mList) {
		this->mail.erase(std::remove(this->mail.begin(), this->mail.end(), m), this->mail.end());
	}
}
std::vector<Mail> MailContainer::getMail() {
	return this->mail;
}

SaveData MailContainer::getSaveData() {
	SaveData d("MailContainer");
	for (Mail mail : this->mail) {
		SaveData m("Mail");
		m.addValue("id", std::to_string(mail.getId()));
		m.addValue("srcX", std::to_string(mail.getSrc().x));
		m.addValue("srcY", std::to_string(mail.getSrc().y));
		m.addValue("destX", std::to_string(mail.getDest().x));
		m.addValue("destY", std::to_string(mail.getDest().y));
		d.addData(m);
	}
	return d;
}
