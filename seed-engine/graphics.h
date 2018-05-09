#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>

#include "window.h"
#include "texture.h"
#include "renderer.h"
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
		virtual ~Graphics(void) {}

	private:
		const std::unique_ptr<Window> & window_;

	public:
		const std::unique_ptr<Window> & window(void) { return this->window_; }

	public:
		std::vector<std::weak_ptr<Renderer>> rendering_list_;

	public:
		virtual void Initialize(void) = 0;
		virtual void Finalize(void) = 0;

	private:
		std::unique_ptr<RenderTarget> render_target_;
		std::unique_ptr<RasterizerState> rasterizer_state_;
		std::unique_ptr<Model> model_;
		std::unique_ptr<Shader> shader_;

	public:
		const std::unique_ptr<RenderTarget> & render_target(void) { return this->render_target_; }
		const std::unique_ptr<RasterizerState> & rasterizer_state(void) { return this->rasterizer_state_; }
		const std::unique_ptr<Model> & model(void) { return this->model_; }
		const std::unique_ptr<Shader> & shader(void) { return this->shader_; }

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
		template<class _Model, class ... Args> void set_model(const Args &... args)
		{
			this->model_ = std::make_unique<_Model>(args ...);
		}
		template<class _Shader, class ... Args> void set_shader(const Args &... args)
		{
			this->shader_ = std::make_unique<_Shader>(args ...);
		}

	public:
		virtual bool Run(void)
		{
			this->render_target_->Clear();

			for (unsigned int n = 0; n < this->rendering_list_.size(); ++n)
			{
				auto & renderer = this->rendering_list_[n];
				if (renderer.expired())
				{
					this->rendering_list_.erase(this->rendering_list_.begin() + n);
				}
				else
				{
					this->Rendering(renderer);
				}
			}

			this->Present();


			this->CalcFps();

			return true;
		}

	private:
		void Rendering(const std::weak_ptr<Renderer> & renderer_weak)
		{
			auto renderer = renderer_weak.lock();
			this->render_target_->Setup(renderer->viewport(), renderer->render_targets(), renderer->depth_stencil());
			this->rasterizer_state_->Setup(renderer->rasterizer_state());
			this->shader_->Setup(renderer->shader_file(), renderer->constant_buffer());
			this->model_->Draw(renderer->model_file());
		}

	public:
		void AddRendererToRenderingList(const std::weak_ptr<Renderer> renderer) { this->rendering_list_.emplace_back(renderer); }

	private:
		virtual void Present(void) = 0;

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
	};
}