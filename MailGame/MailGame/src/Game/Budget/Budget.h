#pragma once
#include <vector>
#include "Constants.h"

enum ExpenseType {
	Entity,
	Route
};

class Expense {
	// The type of expense
	ExpenseType type;
	// The ID of the entity that has this expense
	id_t entity_id;
	// The ID of the route, may be unused
	id_t route_id;
	// The cost of the expense
	unsigned int cost;
};

class Budget {
private:
	// How much income the player has
	unsigned int income;
	// What the player is spending their income on
	std::vector<Expense> expenses;

public:
	unsigned int getExcessIncome();
	void addExpense(Expense expense);
	void setIncome(unsigned int income);
	unsigned int getIncome();
};
