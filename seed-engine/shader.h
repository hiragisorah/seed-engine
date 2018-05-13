#pragma once

#include <string>

#include "camera.h"

namespace Seed
{
	class Shader
	{
	public:
		virtual ~Shader(void) {}

	public:
		virtual void Load(std::string name) = 0;
		virtual void Unload(std::string name) = 0;
		virtual void Setup(std::string name, const std::vector<void*> & constant_buffers) = 0;
	};
}