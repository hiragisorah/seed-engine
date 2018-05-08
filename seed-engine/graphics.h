#pragma once

#include <memory>
#include <string>
#include <vector>

#include "window.h"
#include "texture.h"
#include "geometry.h"
#include "shader.h"
#include "model.h"
#include "render-target.h"

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
		std::vector<std::weak_ptr<Model>> model_list_;

	public:
		virtual void Initialize(void) = 0;
		virtual void Finalize(void) = 0;

	private:
		std::unique_ptr<RenderTarget> render_target_;

	public:
		template<class _RenderTarget, class ... Args> void set_render_target(const Args &... args)
		{
			this->render_target_ = std::make_unique<_RenderTarget>(args ...);
			this->render_target_->Initialize();
		}

	public:
		virtual bool Run(void)
		{
			this->render_target_->Clear();

			for (auto && model : this->model_list_)
			{
				this->render_target_->Setup(model.lock()->viewport(), model.lock()->render_targets(), model.lock()->depth_stencil());
				this->RenderModel(model);
			}

			this->Present();

			return true;
		}

	private:
		virtual void Present(void) = 0;
		virtual void RenderModel(const std::weak_ptr<Model> & model) = 0;

	public:
		void AddModelToRenderingList(const std::weak_ptr<Model> model) { this->model_list_.emplace_back(model); }

	public:
		virtual const std::shared_ptr<Texture> CreateTexture(std::string file_path) = 0;
		virtual const std::shared_ptr<Geometry> CreateGeometry(std::string file_path) = 0;
		virtual const std::shared_ptr<Shader> CreateShader(std::string file_path) = 0;
	};
}