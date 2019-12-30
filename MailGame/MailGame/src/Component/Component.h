#pragma once
#include <memory>
// Forward declaration of entity
class Entity;

// A component in an entity
class Component {
public:
	// Set/Get Entity
	void setEntity(std::shared_ptr<Entity> e);
	std::shared_ptr<Entity> getEntity();
private:
	std::weak_ptr<Entity> entity;
};
