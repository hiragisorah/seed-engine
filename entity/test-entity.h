#pragma once

#include <seed-engine\entity.h>
#include <component\test-component.h>

class TestEntity : public Seed::Entity
{
public:
	TestEntity(const std::shared_ptr<Seed::Scene> & scene, const std::shared_ptr<Seed::Entity> & parent, std::shared_ptr<Seed::Entity> & self)
		: Entity(scene, parent, self) {}

public:
	void Initialize(void)
	{
		this->add_component<TestComponent>();
	}
};