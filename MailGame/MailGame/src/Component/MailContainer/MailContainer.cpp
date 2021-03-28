#include "MailContainer.h"
#include "System/SaveData/SaveData.h"
#include "Component/ComponentType/ComponentType.h"
#include <algorithm>
#include <stdexcept>

void MailContainer::addMail(std::vector<Mail> toAdd) {
	this->mail.insert(this->mail.end(), toAdd.begin(), toAdd.end());
}
void MailContainer::transferAllMailTo(std::shared_ptr<MailContainer> o, MailEvent e) {
	transferAllMailTo(o, std::optional<MailEvent>(e));
}
void MailContainer::transferAllMailTo(std::shared_ptr<MailContainer> other, std::optional<MailEvent> e) {
	if (e.has_value()) {
		for (auto it = this->mail.begin(); it != this->mail.end(); it++) {
			it->addEvent(e.value());
		}
	}
	other->addMail(this->mail);
	this->mail = {};
}
void MailContainer::transferSomeMailTo(std::vector<Mail> t, std::shared_ptr<MailContainer> o, MailEvent e) {
	transferSomeMailTo(t, o, std::optional<MailEvent>(e));
}
void MailContainer::transferSomeMailTo(std::vector<Mail> toGive, std::shared_ptr<MailContainer> other, std::optional<MailEvent> e) {
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
	if (e.has_value()) {
		for (auto it = toGive.begin(); it != toGive.end(); it++) {
			it->addEvent(e.value());
		}
	}
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

std::optional<SaveData> MailContainer::getSaveData() {
	SaveData d(componentTypeToStr(ComponentType::MailContainer));
	for (Mail mail : this->mail) {
		d.addData(mail.getSaveData());
	}
	return d;
}
void MailContainer::fromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		this->mail.push_back(Mail(d));
	}
}