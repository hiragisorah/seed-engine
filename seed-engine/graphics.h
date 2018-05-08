#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>

#include "window.h"
#include "texture.h"
#include "geometry.h"
#include "shader.h"
#include "model.h"
#include "render-target.h"
#include "rasterizer-state.h"

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
		std::unique_ptr<RasterizerState> rasterizer_state_;

	public:
		template<class _RenderTarget, class ... Args> void set_render_target(const Args &... args)
		{
			this->render_target_ = std::make_unique<_RenderTarget>(args ...);
			this->render_target_->Initialize();
		}
		template<class _RasterizerState, class ... Args> void set_rasterizer_state(const Args &... args)
		{
			this->rasterizer_state_ = std::make_unique<_RasterizerState>(args ...);
			this->rasterizer_state_->Initialize();
		}

	private:
		unsigned int fps_;
		std::chrono::steady_clock::time_point start_timer_;

	private:
		void CalcFps(void)
		{
			auto now_timer = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now_timer - this->start_timer_);
			this->fps_++;
			if (diff.count() >= 1000)
			{
				printf("%6d\r", this->fps_);
				this->fps_ = 0;
				this->start_timer_ = now_timer;
			}
		}

	public:
		virtual bool Run(void)
		{
			this->render_target_->Clear();

			for (unsigned int n = 0; n < this->model_list_.size(); ++n)
			{
				auto & model = this->model_list_[n];
				if (model.expired())
				{
					this->model_list_.erase(this->model_list_.begin() + n);
				}
				else
				{
					this->render_target_->Setup(model.lock()->viewport(), model.lock()->render_targets(), model.lock()->depth_stencil());
					this->rasterizer_state_->Setup(model.lock()->rasterizer_state());
					this->RenderModel(model);
				}
			}

			this->Present();

			this->CalcFps();

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