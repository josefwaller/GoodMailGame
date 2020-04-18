#pragma once
#include <string>
#include <map>

#define ENTITY_TAGS \
	F(Residence) \
	F(PostOffice) \
	F(MailBox) \
	F(MailTruck) \
	F(CargoTruckDepot) \
	F(CargoTruck) \
	F(TrainStation) \
	F(Train)

#define F(x) x,
enum class EntityTag {
	ENTITY_TAGS
};

#undef F
#define F(x) { EntityTag::x, #x },

const std::map<EntityTag, std::string> tagStrs = {
	ENTITY_TAGS
};

std::string entityTagToString(EntityTag t);

#undef F
#undef ENTITY_TAGS
