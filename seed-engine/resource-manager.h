#pragma once

#include <memory>
#include <unordered_map>

#include "graphics.h"
#include "texture.h"

namespace Seed
{
	class ResourceManager
	{
	public:
		ResourceManager(const std::unique_ptr<Graphics> & graphics) : graphics_(graphics) {}

	private:
		const std::unique_ptr<Graphics> & graphics_;

	public:
		const std::unique_ptr<Graphics> & graphics(void) { return this->graphics_; }

	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;

	public:
		void LoadTexture(const std::string & file_name)
		{
			auto file_path = "resource/texture/" + file_name;
			auto texture = this->graphics_->CreateTexture(file_path);
			this->textures_[file_name] = texture;
		}
	};
}