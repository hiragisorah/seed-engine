#pragma once

#include <seed-engine\entity.h>

#include <component\lens.h>

class TestCamera : public Seed::Entity
{
public:
	TestCamera(std::shared_ptr<Entity> & self)
		: Entity(self)
	{}

private:
	void Initialize(void) override
	{
		this->add_component<Lens>();
	}
};