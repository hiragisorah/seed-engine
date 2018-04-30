#pragma once

#include <seed-engine\component.h>

class TestComponent : public Seed::Component
{
public:
	TestComponent(const std::shared_ptr<Seed::Entity> & entity, std::shared_ptr<Seed::Component> & self)
		: Component(entity, self) {}

public:
	void Initialize(void) override
	{
		printf("%s\n", __FUNCTION__);
	}
	void Update(void) override
	{
		printf("%s\n", __FUNCTION__);
	}
};