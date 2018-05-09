#pragma once

namespace Seed
{
	enum RS
	{
		RS_CULL_NONE = 0,
		RS_CW,
		RS_CCW,
		RS_WIREFRAME,
		RS_NUM
	};

	class RasterizerState
	{
	public:
		void Initialize(void)
		{
			this->create_cull_none();
			this->create_cull_clock_wise();
			this->create_cull_counter_clock_wise();
			this->create_wireframe();
		}

	private:
		virtual void create_cull_none(void) = 0;
		virtual void create_cull_clock_wise(void) = 0;
		virtual void create_cull_counter_clock_wise(void) = 0;
		virtual void create_wireframe(void) = 0;

	public:
		virtual void Setup(const RS & rasterizer_state) = 0;
	};
}