#pragma once

#include <memory>

#include <DirectXMath.h>

#include <seed-engine\graphics.h>
#include <seed-engine\component.h>
#include <seed-engine\renderer.h>

#include <component\lens.h>
#include <scene\test-scene.h>

class TestComponent final : public Seed::Component
{
	struct CBUFFER
	{
		DirectX::XMMATRIX world_;
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
		auto camera_cbuffer = this->entity().lock()->scene().lock()->system<CameraManager>()->main_camera().lock()->component<Lens>()->constant_buffer();

		this->cbuffer_.world_ = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);

		this->renderer_ = std::make_shared<Seed::Renderer>();

		this->renderer_->set_constant_buffer(0, &this->cbuffer_);
		this->renderer_->set_constant_buffer(1, &camera_cbuffer);
		this->renderer_->set_rasterizer_state(RS_CW);
		this->renderer_->set_render_targets({ RT_COLOR_MAP, RT_POSITION_MAP, RT_NORMAL_MAP, RT_DEPTH_MAP });
		this->renderer_->set_depth_stencil(DS_DEFFERED);
		this->renderer_->set_shader_file("simple3d-deffered");
		this->renderer_->set_model_file("kaoru");

		auto & graphics = this->entity().lock()->scene().lock()->graphics();

		graphics.lock()->AddRendererToRenderingList(this->renderer_);
	}
	void Update(void) override
	{
		this->cbuffer_.world_ *= DirectX::XMMatrixRotationY(.01f);
	}
};