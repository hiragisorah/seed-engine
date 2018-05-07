#pragma once

#include <seed-engine\scene.h>
#include <entity\test-entity.h>

class TestScene : public Seed::Scene
{
public:
	TestScene(std::shared_ptr<Seed::Scene> & self) : Scene(self) {}

public:
	void Initialize(void) override
	{
		this->resource_manager()->load_geometry("test.geometry");
		this->resource_manager()->load_shader("simple-deffered.hlsl");

		this->Create<TestEntity>();
	}
	void Finalize(void) override
	{
		this->resource_manager()->unload_geometry("test.geometry");
		this->resource_manager()->unload_shader("simple-deffered.hlsl");
	}
};