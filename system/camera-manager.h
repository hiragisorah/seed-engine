#pragma once

#include <memory>

#include <DirectXMath.h>

#include <seed-engine\system.h>
#include <seed-engine\entity.h>

class CameraManager : public Seed::System
{
public:
	CameraManager(std::shared_ptr<System> & self)
		: System(self)
	{}

private:
	std::weak_ptr<Seed::Entity> main_camera_;

public:
	const std::weak_ptr<Seed::Entity> & main_camera(void) const { return this->main_camera_; }
	void set_main_camera(const std::weak_ptr<Seed::Entity> & camera) { this->main_camera_ = camera; }
};