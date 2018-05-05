#pragma once

#include <memory>
#include <string>
#include <vector>

#include "window.h"
#include "texture.h"
#include "geometry.h"
#include "shader.h"
#include "model.h"

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
		virtual void AddModelToRenderingList(const std::shared_ptr<Model> & model, int list_num) {}
		virtual void RenderModel(const std::weak_ptr<Model> & model) {}

	public:
		virtual const std::shared_ptr<Texture> CreateTexture(std::string file_path) { return std::make_shared<Texture>(); }
		virtual const std::shared_ptr<Geometry> CreateGeometry(std::string file_path) { return std::make_shared<Geometry>(); }
		virtual const std::shared_ptr<Shader> CreateShader(std::string file_path) { return std::make_shared<Shader>(); }

	private:
		virtual void CreateBackBuffer(void) {}
		virtual void CreateDeffered(void) {}
	};
}