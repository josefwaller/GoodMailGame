#pragma once
#include <memory>
#include <optional>
// Forward declaration of entity
class Entity;
class SaveData;

// A component in an entity
class Component {
public:
	// Initialization method
	// Called when it is safe to initialize, since when loading we need to construct entities without initializing them
	// Currently not called when loading, usually they are initialized in fromSaveData
	virtual void init();
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
