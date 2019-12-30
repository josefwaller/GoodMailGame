#pragma once
#include <memory>
// Forward declaration of entity
class Entity;

// A component in an entity
class Component {
public:
	Component(std::weak_ptr<Entity> entity);
	std::shared_ptr<Entity> getEntity();
private:
	std::weak_ptr<Entity> entity;
};
