#include "MailContainer.h"
#include <algorithm>

void MailContainer::addMail(std::vector<Mail> toAdd) {
	this->mail.insert(this->mail.end(), toAdd.begin(), toAdd.end());
}
void MailContainer::transferAllMailTo(std::shared_ptr<MailContainer> other) {
	other->addMail(this->mail);
	this->mail = {};
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
