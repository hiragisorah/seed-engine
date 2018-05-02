#pragma once

#include <memory>
#include <unordered_map>

#include "graphics.h"
#include "texture.h"
#include "shader.h"

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
		std::unordered_map<std::string, std::shared_ptr<Model>> models_;
		std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;

	public:
		void load_texture(const std::string & file_name)
		{
			auto file_path = "resource/texture/" + file_name;
			auto texture = this->graphics_->CreateTexture(file_path);
			this->textures_[file_name] = texture;
		}
		void load_model(const std::string & file_name)
		{
			auto file_path = "resource/model/" + file_name;
			auto model = this->graphics_->CreateModel(file_path);
			this->models_[file_name] = model;
		}
		void load_shader(const std::string & file_name)
		{
			auto file_path = "resource/shader/" + file_name;
			auto shader = this->graphics_->CreateShader(file_path);
			this->shaders_[file_name] = shader;
		}

	public:
		void unload_texture(const std::string & file_name)
		{
			this->textures_[file_name].reset();
		}
		void unload_model(const std::string & file_name)
		{
			this->models_[file_name].reset();
		}
		void unload_shader(const std::string & file_name)
		{
			this->shaders_[file_name].reset();
		}

	public:
		const std::shared_ptr<Texture> & texture(const std::string & file_name)
		{
			return this->textures_[file_name];
		}
		const std::shared_ptr<Model> & model(const std::string & file_name)
		{
			return this->models_[file_name];
		}
		const std::shared_ptr<Shader> & shader(const std::string & file_name)
		{
			return this->shaders_[file_name];
		}
	};
}