#pragma once

#include <string>

namespace Seed
{
	class Model
	{
	public:
		virtual ~Model(void) {}

	public:
		virtual void Load(std::string name) = 0;
		virtual void Unload(std::string name) = 0;
		virtual void Draw(std::string name) = 0;
	};
}