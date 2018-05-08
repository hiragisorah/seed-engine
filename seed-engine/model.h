#pragma once

#include <memory>

#include "geometry.h"
#include "shader.h"
#include "render-target.h"

namespace Seed
{
	class Model
	{
	public:
		Model(void)
			: viewport_(VP_SIMPLE)
			, depth_stencil_(DS_SIMPLE)
			, render_targets_({RT_BACKBUFFER})
			{}

	private:
		VP viewport_;
		DS depth_stencil_;
		std::vector<RT> render_targets_;

	public:
		const VP & viewport(void) const { return this->viewport_; }
		const DS & depth_stencil(void) const { return this->depth_stencil_; }
		const std::vector<RT> & render_targets(void) const { return this->render_targets_; }

	private:
		std::weak_ptr<Geometry> geometry_;
		std::weak_ptr<Shader> shader_;
		void * constant_buffer_ = nullptr;
		unsigned int priority_ = 0;

	public:
		template<class _Geometry> const std::weak_ptr<_Geometry> geometry(void) const
		{
			return std::static_pointer_cast<_Geometry>(this->geometry_.lock());
		}
		template<class _Shader> const std::weak_ptr<_Shader> shader(void) const
		{
			return std::static_pointer_cast<_Shader>(this->shader_.lock());
		}
		void * const constant_buffer(void) const
		{
			return this->constant_buffer_;
		}
		const unsigned int priority(void) const
		{
			return this->priority_;
		}

	public:
		void set_geometry(const std::shared_ptr<Geometry> & geometry)
		{
			this->geometry_ = geometry;
		}
		void set_shader(const std::shared_ptr<Shader> & shader)
		{
			this->shader_ = shader;
		}
		void set_constant_buffer(void * constant_buffer)
		{
			this->constant_buffer_ = constant_buffer;
		}
		void set_priority(unsigned int priority)
		{
			this->priority_ = priority;
		}
	};
}