#pragma once

#include <DirectXMath.h>

#include <seed-engine\component.h>

class Lens : public Seed::Component
{
public:
	struct CBUFFER
	{
		DirectX::XMMATRIX view_;
		DirectX::XMMATRIX proj_;
	};

public:
	Lens(std::shared_ptr<Component> & self)
		: Component(self)
		, fov_(DirectX::XM_PIDIV4)
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

private:
	void Always(void) override
	{
		this->constant_buffer_.view_ = DirectX::XMMatrixLookAtLH(this->eye_, this->at_, this->up_);
		this->constant_buffer_.proj_ = DirectX::XMMatrixPerspectiveFovLH(this->fov_, this->aspect_, this->near_, this->far_);
	}
};