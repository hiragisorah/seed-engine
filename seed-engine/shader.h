#pragma once

#include <string>

namespace Seed
{
	class Shader
	{
	public:
		virtual ~Shader(void) {}

	public:
		virtual void Load(std::string name) = 0;
		virtual void Unload(std::string name) = 0;
		virtual void Setup(std::string name, void * constant_buffer) = 0;
	};
}