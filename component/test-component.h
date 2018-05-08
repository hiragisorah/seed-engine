#pragma once

#include <memory>

#include <DirectXTK\GeometricPrimitive.h>

#include <seed-engine\graphics.h>
#include <seed-engine\component.h>
#include <seed-engine\model.h>

class TestComponent final : public Seed::Component
{
	struct CBUFFER
	{
		DirectX::XMMATRIX world_;
		DirectX::XMMATRIX view_;
		DirectX::XMMATRIX proj_;
	};

private:
	std::shared_ptr<Seed::Model> model_;
	CBUFFER cbuffer_;

public:
	TestComponent(std::shared_ptr<Seed::Component> & self)
		: Component(self) {}

public:
	void OnAdd(void) override
	{
		auto & resource_manager = this->entity().lock()->scene().lock()->resource_manager();
		auto & graphics         = resource_manager->graphics();
		auto geometry           = resource_manager->geometry("hand.geometry");
		auto shader             = resource_manager->shader("simple-deffered.hlsl");
		this->cbuffer_.world_   = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);
		this->cbuffer_.view_    = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 5.f, -5.f, 0), DirectX::XMVectorZero(), DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f));
		this->cbuffer_.proj_    = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 16.f / 9.f, 0.1f, 1000.f);
		this->model_            = std::make_shared<Seed::Model>();

		this->model_->set_constant_buffer(&this->cbuffer_);
		this->model_->set_geometry(geometry);
		this->model_->set_shader(shader);

		graphics->AddModelToRenderingList(this->model_);
	}
	void Update(void) override
	{
		this->cbuffer_.world_ *= DirectX::XMMatrixRotationY(0.01f);
	}
};