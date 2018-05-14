#pragma once

#include <seed-engine\scene.h>

#include <system\camera-manager.h>
#include <entity\test-entity.h>
#include <entity\test-camera.h>

using namespace Seed;

class TestScene : public Scene
{
public:
	TestScene(std::shared_ptr<Scene> & self)
		: Scene(self)
	{}

public:
	void Initialize(void) override
	{
		this->graphics().lock()->model()->Load("jeep");
		this->graphics().lock()->model()->Load("hand");
		this->graphics().lock()->model()->Load("quad2d");
		this->graphics().lock()->model()->Load("kaoru");
		this->graphics().lock()->shader()->Load("simple2d-backbuffer");
		this->graphics().lock()->shader()->Load("simple3d-backbuffer");
		this->graphics().lock()->shader()->Load("simple3d-deffered");

		this->add_system<CameraManager>();

		auto & camera = this->Create<TestCamera>();
		this->system<CameraManager>()->set_main_camera(camera);

		this->Create<TestEntity>();
	}
	void Finalize(void) override
	{
		this->graphics().lock()->model()->Unload("jeep");
		this->graphics().lock()->model()->Unload("hand");
		this->graphics().lock()->model()->Unload("quad2d");
		this->graphics().lock()->model()->Unload("kaoru");
		this->graphics().lock()->shader()->Unload("simple2d-backbuffer");
		this->graphics().lock()->shader()->Unload("simple3d-backbuffer");
		this->graphics().lock()->shader()->Unload("simple3d-deffered");
	}
};