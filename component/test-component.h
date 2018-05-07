#pragma once

#include <memory>

#include <DirectXTK\GeometricPrimitive.h>

#include <seed-engine\graphics.h>
#include <seed-engine\component.h>
#include <seed-engine\model.h>

class TestComponent final : public Seed::Component
{
private:
	std::shared_ptr<Seed::Model> model_;
	DirectX::XMMATRIX world_;

public:
	TestComponent(std::shared_ptr<Seed::Component> & self)
		: Component(self) {}

public:
	void OnAdd(void) override
	{
		auto & resource_manager = this->entity().lock()->scene().lock()->resource_manager();
		auto geometry = resource_manager->geometry("test.geometry");
		auto shader = resource_manager->shader("test.hlsl");

		this->model_ = std::make_shared<Seed::Model>();
		this->model_->set_constant_buffer(&this->world_);
		this->model_->set_geometry(geometry);
		this->model_->set_shader(shader);

		auto proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 16.f / 9.f, 0.1f, 1000.f);
		auto view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 5.f, -5.f, 0), DirectX::XMVectorZero(), DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f));
		this->world_ = DirectX::XMMatrixScaling(.01f, .01f, .01f) * view * proj;
	}
	void Update(void) override
	{

	}
	void Render(const std::unique_ptr<Seed::Graphics> & graphics) override
	{
		graphics->AddModelToRenderingList(this->model_, 0);
	}
};