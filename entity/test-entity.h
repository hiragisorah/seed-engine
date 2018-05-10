#pragma once

#include <seed-engine\entity.h>
#include <component\test-component.h>
#include <component\g-buffer.h>

class TestEntity : public Seed::Entity
{
public:
	TestEntity(std::shared_ptr<Seed::Entity> & self)
		: Entity(self) {}

public:
	void Initialize(void)
	{
		this->add_component<TestComponent>();
		this->add_component<GBuffer>();
	}
};