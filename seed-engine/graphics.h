#pragma once

#include <memory>

#include "window.h"
#include "texture.h"
#include "model.h"
#include "shader.h"

namespace Seed
{
	class Graphics
	{
	public:
		Graphics(const std::unique_ptr<Window> & window) : window_(window) {}

	private:
		const std::unique_ptr<Window> & window_;

	public:
		const std::unique_ptr<Window> & window(void) { return this->window_; }

	public:
		virtual ~Graphics(void) {}

	public:
		virtual void Initialize(void) {}
		virtual bool Run(void) { return true; }
		virtual void Finalize(void) {}

	public:
		virtual const std::shared_ptr<Texture> CreateTexture(std::string file_path) { return std::make_shared<Texture>(); }
		virtual const std::shared_ptr<Model> CreateModel(std::string file_path) { return std::make_shared<Model>(); }
		virtual const std::shared_ptr<Shader> CreateShader(std::string file_path) { return std::make_shared<Shader>(); }
	};
}