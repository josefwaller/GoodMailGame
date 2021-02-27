#pragma once
#include <SFML/System/Vector3.hpp>
#include <optional>

// A point with a maximum speed
class SpeedPoint {
public:
	SpeedPoint(sf::Vector3f);
	SpeedPoint(sf::Vector3f pos, float speed);
	std::optional<float> getSpeed();
	void setSpeed(float s);
	sf::Vector3f getPos();
private:
	sf::Vector3f pos;
	// If speed is unset, should just default to the vehicle's speed
	std::optional<float> speed;
};
