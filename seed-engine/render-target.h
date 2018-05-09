#pragma once

#include <vector>

namespace Seed
{
	enum RT
	{
		RT_BACKBUFFER,
		RT_COLOR_MAP,
		RT_POSITION_MAP,
		RT_NORMAL_MAP,
		RT_DEPTH_MAP,
		RT_SHADOW_MAP,
		RT_NUM
	};

	enum VP
	{
		VP_SIMPLE = 0,
		VP_SHADOW_MAP,
		VP_NUM
	};

	enum DS
	{
		DS_SIMPLE = 0,
		DS_SHADOW_MAP,
		DS_NUM
	};

	class RenderTarget
	{
	public:
		virtual ~RenderTarget(void) {}

	public:
		void Initialize(void)
		{
			this->create_simple_viewport();
			this->create_shadow_map_viewport();

			this->create_backbuffer();
			this->create_color_map();
			this->create_position_map();
			this->create_normal_map();
			this->create_depth_map();
			this->create_shadow_map();

			this->create_simple_depth_stencil();
			this->create_shadow_map_depth_stencil();
		}

	private:
		virtual void create_simple_viewport(void) = 0;
		virtual void create_shadow_map_viewport(void) = 0;
		virtual void create_backbuffer(void) = 0;
		virtual void create_color_map(void) = 0;
		virtual void create_position_map(void) = 0;
		virtual void create_normal_map(void) = 0;
		virtual void create_depth_map(void) = 0;
		virtual void create_shadow_map(void) = 0;
		virtual void create_simple_depth_stencil(void) = 0;
		virtual void create_shadow_map_depth_stencil(void) = 0;

	public:
		virtual void Clear(void) = 0;
		virtual void SetupViewport(const VP & vp) = 0;
		virtual void SetupRenderTargetsAndDepthStencil(const std::vector<RT> & rts, const DS & ds) = 0;
		virtual void SetupTextures(const std::vector<RT> & rts) = 0;

	public:
		void Setup(const VP & vp, const std::vector<RT> & rts, const DS & ds)
		{
			this->SetupViewport(vp);
			this->SetupRenderTargetsAndDepthStencil(rts, ds);
		}
	};
}