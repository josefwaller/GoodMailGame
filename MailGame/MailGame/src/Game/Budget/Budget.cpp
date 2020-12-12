#include "Budget.h"

void Budget::addExpense(Expense e) {
	this->expenses.push_back(e);
}

unsigned int Budget::getIncome() {
	return this->income;
}

void Budget::setIncome(unsigned int income) {
	this->income = income;
}

unsigned int Budget::getExcessIncome() {
	unsigned int inc = this->income;

	for (Expense e : this->expenses) {
		inc -= e.cost;
	}
}