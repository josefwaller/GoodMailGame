#pragma once
#include <memory>
#include <optional>
// Forward declaration of entity
class Entity;
class SaveData;

// A component in an entity
class Component {
public:
	// Method ran on deletion
	virtual void onDelete();
	// Set/Get Entity
	void setEntity(std::shared_ptr<Entity> e);
	std::shared_ptr<Entity> getEntity();
	virtual std::optional<SaveData> getSaveData();
	// Load the component from save data, after the entities have all been initialized
	virtual void fromSaveData(SaveData d);
private:
	std::weak_ptr<Entity> entity;
};
