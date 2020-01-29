#include "MailContainer.h"
#include <algorithm>

void MailContainer::addMail(std::vector<Mail> toAdd) {
	this->mail.insert(this->mail.end(), toAdd.begin(), toAdd.end());
}
void MailContainer::transferAllMailTo(std::shared_ptr<MailContainer> other) {
	other->addMail(this->mail);
	this->mail = {};
}
