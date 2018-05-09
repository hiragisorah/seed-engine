#pragma once

#include <memory>

#include <seed-engine\graphics.h>
#include <seed-engine\component.h>
#include <seed-engine\renderer.h>

class TestComponent final : public Seed::Component
{
	struct CBUFFER
	{
		DirectX::XMMATRIX world_;
		DirectX::XMMATRIX view_;
		DirectX::XMMATRIX proj_;
	};

private:
	std::shared_ptr<Seed::Renderer> renderer_;
	CBUFFER cbuffer_;

public:
	TestComponent(std::shared_ptr<Seed::Component> & self)
		: Component(self) {}

public:
	void OnAdd(void) override
	{
		this->cbuffer_.world_ = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);
		this->cbuffer_.view_  = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 5.f, -5.f, 0), DirectX::XMVectorZero(), DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f));
		this->cbuffer_.proj_  = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 16.f / 9.f, 0.1f, 1000.f);
		this->renderer_       = std::make_shared<Seed::Renderer>();

		this->renderer_->set_constant_buffer(&this->cbuffer_);

		auto & graphics = this->entity().lock()->scene().lock()->graphics();

		graphics->AddRendererToRenderingList(this->renderer_);
	}
	void Update(void) override
	{
		this->cbuffer_.world_ *= DirectX::XMMatrixRotationY(0.01f);
	}
};