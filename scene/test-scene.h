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
		this->graphics()->model()->Load("jeep");
		this->graphics()->model()->Load("hand");
		this->graphics()->model()->Load("quad2d");
		this->graphics()->shader()->Load("simple2d-backbuffer");
		this->graphics()->shader()->Load("simple3d-backbuffer");
		this->graphics()->shader()->Load("simple3d-deffered");

		this->Create<TestEntity>();
	}
	void Finalize(void) override
	{
		this->graphics()->model()->Unload("jeep");
		this->graphics()->model()->Unload("hand");
		this->graphics()->model()->Unload("quad2d");
		this->graphics()->shader()->Unload("simple2d-backbuffer");
		this->graphics()->shader()->Unload("simple3d-backbuffer");
		this->graphics()->shader()->Unload("simple3d-deffered");
	}
};