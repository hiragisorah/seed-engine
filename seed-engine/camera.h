#pragma once

#include <DirectXMath.h>

namespace Seed
{
	class Camera
	{
	public:
		struct CBUFFER
		{
			DirectX::XMMATRIX view_;
			DirectX::XMMATRIX proj_;
		};

	public:
		Camera(void)
			: fov_(DirectX::XM_PIDIV4)
			, aspect_(16.f / 9.f)
			, near_(0.1f)
			, far_(100.f)
			, eye_(DirectX::XMVectorSet(0.f, .5f, .5f, 0.f))
			, at_(DirectX::XMVectorZero())
			, up_(DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f))
		{}

	private:
		float fov_;
		float aspect_;
		float near_;
		float far_;

		DirectX::XMVECTOR eye_;
		DirectX::XMVECTOR at_;
		DirectX::XMVECTOR up_;

		CBUFFER constant_buffer_;

	public:
		const float & fov(void) const { return this->fov_; }
		const float & aspect(void) const { return this->aspect_; }
		const float & _near(void) const { return this->near_; }
		const float & _far(void) const { return this->far_; }

		const CBUFFER & constant_buffer(void) const { return this->constant_buffer_; }

		void set_fov(const float & fov) { this->fov_ = fov; }
		void set_aspect(const float & aspect) { this->aspect_ = aspect; }
		void set_near(const float & _near) { this->near_ = _near; }
		void set_far(const float & _far) { this->far_ = _far; }

		void set_constant_buffer(const CBUFFER & constant_buffer) { this->constant_buffer_ = constant_buffer; }
	};
}