#pragma once

#include <seed-engine\scene.h>

#include <entity\test-entity.h>

using namespace Seed;

class TestScene : public Scene
{
public:
	TestScene(std::shared_ptr<Scene> & self, const std::unique_ptr<Graphics> & graphics)
		: Scene(self, graphics)
	{}

public:
	void Initialize(void) override
	{
		this->graphics()->model()->Load("hand");
		this->graphics()->shader()->Load("simple-deffered");

		this->Create<TestEntity>();
	}
	void Finalize(void) override
	{
		this->graphics()->model()->Unload("hand");
		this->graphics()->shader()->Unload("simple-deffered");
	}
};