#include "Mail/Mail.h"

bool Mail::operator==(Mail other) {
	return this->dest == other.dest && this->src == other.src;
}