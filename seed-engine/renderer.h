#pragma once

#include <memory>
#include <string>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "render-target.h"
#include "rasterizer-state.h"

namespace Seed
{
	class Renderer
	{
	public:
		Renderer(void)
			: rasterizer_state_(RS_CW)
			, viewport_(VP_SIMPLE)
			, depth_stencil_(DS_SIMPLE)
			, render_targets_({ RT_BACKBUFFER })
			, setup_textures_({ RT_NONE })
			, shader_file_("simple3d-backbuffer")
			, model_file_("jeep")
			, constant_buffer_(nullptr)
			, priority_(0)
		{}

	private:
		std::weak_ptr<Camera> camera_;
		RS rasterizer_state_;
		VP viewport_;
		DS depth_stencil_;
		std::vector<RT> render_targets_;
		std::vector<RT> setup_textures_;
		std::string shader_file_;
		std::string model_file_;
		void * constant_buffer_;
		unsigned int priority_;

	public:
		const RS & rasterizer_state(void) const { return this->rasterizer_state_; }
		const VP & viewport(void) const { return this->viewport_; }
		const DS & depth_stencil(void) const { return this->depth_stencil_; }
		const std::vector<RT> & render_targets(void) const { return this->render_targets_; }
		const std::vector<RT> & setup_textures(void) const { return this->setup_textures_; }
		const std::string & shader_file(void) const { return this->shader_file_; }
		const std::string & model_file(void) const { return this->model_file_; }
		void * const constant_buffer(void) const { return this->constant_buffer_;}
		const unsigned int priority(void) const { return this->priority_; }
		const std::weak_ptr<Camera> camera(void) const { return this->camera_; }

		void set_rasterizer_state(const RS & rasterizer_state) { this->rasterizer_state_ = rasterizer_state; }
		void set_viewport(const VP & viewport) { this->viewport_ = viewport; }
		void set_depth_stencil(const DS & depth_stencil) { this->depth_stencil_ = depth_stencil; }
		void set_render_targets(const std::vector<RT> & render_targets) { this->render_targets_ = render_targets; }
		void set_setup_textures(const std::vector<RT> & setup_textures) { this->setup_textures_ = setup_textures; }
		void set_shader_file(const std::string & shader_file) { this->shader_file_ = shader_file; }
		void set_model_file(const std::string & model_file) { this->model_file_ = model_file; }
		void set_constant_buffer(void * constant_buffer) { this->constant_buffer_ = constant_buffer; }
		void set_priority(unsigned int priority) { this->priority_ = priority; }
		void set_camera(const std::weak_ptr<Camera> & camera) { this->camera_ = camera; }
	};
}