#pragma once

#include <DirectXMath.h>

namespace Seed
{
	class Camera
	{
	public:
		Camera(void)
			: fov_(DirectX::XM_PIDIV4)
			, aspect_(16.f / 9.f)
			, near_(0.1f)
			, far_(100.f)
			, eye_(DirectX::XMVectorZero())
			, at_(DirectX::XMVectorZero())
			, up_(DirectX::XMVectorZero())
		{}

	private:
		float fov_;
		float aspect_;
		float near_;
		float far_;

		DirectX::XMVECTOR eye_;
		DirectX::XMVECTOR at_;
		DirectX::XMVECTOR up_;

	public:
		const float & fov(void) { return this->fov_; }
		const float & aspect(void) { return this->aspect_; }
		const float & _near(void) { return this->near_; }
		const float & _far(void) { return this->far_; }
		void set_fov(const float & fov) { this->fov_ = fov; }
		void set_aspect(const float & aspect) { this->aspect_ = aspect; }
		void set_near(const float & _near) { this->near_ = _near; }
		void set_far(const float & _far) { this->far_ = _far; }
	};
}