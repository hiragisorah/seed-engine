#pragma once

#include <memory>
#include <string>

#include "model.h"
#include "shader.h"
#include "render-target.h"
#include "rasterizer-state.h"

namespace Seed
{
	class Renderer
	{
	public:
		Renderer(void)
			: rasterizer_state_(RS_WIREFRAME)
			, viewport_(VP_SIMPLE)
			, depth_stencil_(DS_SIMPLE)
			, render_targets_({ RT_BACKBUFFER })
			, shader_file_("simple-deffered")
			, model_file_("hand")
			, constant_buffer_(nullptr)
			, priority_(0)
		{}

	private:
		RS rasterizer_state_;
		VP viewport_;
		DS depth_stencil_;
		std::vector<RT> render_targets_;
		std::string shader_file_;
		std::string model_file_;
		void * constant_buffer_;
		unsigned int priority_;

	public:
		const RS & rasterizer_state(void) const { return this->rasterizer_state_; }
		const VP & viewport(void) const { return this->viewport_; }
		const DS & depth_stencil(void) const { return this->depth_stencil_; }
		const std::vector<RT> & render_targets(void) const { return this->render_targets_; }
		const std::string & shader_file(void) const { return this->shader_file_; }
		const std::string & model_file(void) const { return this->model_file_; }
		void * const constant_buffer(void) const { return this->constant_buffer_;}
		const unsigned int priority(void) const { return this->priority_; }
		void set_constant_buffer(void * constant_buffer) { this->constant_buffer_ = constant_buffer; }
		void set_priority(unsigned int priority) { this->priority_ = priority; }
	};
}