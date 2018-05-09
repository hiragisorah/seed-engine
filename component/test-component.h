#pragma once

#include <memory>

#include <DirectXMath.h>

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
	struct CBUFFER2
	{
		DirectX::XMFLOAT2 viewport_;
	};

private:
	std::shared_ptr<Seed::Renderer> renderer_;
	std::shared_ptr<Seed::Renderer> deffered_;
	CBUFFER cbuffer_;
	CBUFFER2 cbuffer2_;

public:
	TestComponent(std::shared_ptr<Seed::Component> & self)
		: Component(self) {}

public:
	void OnAdd(void) override
	{
		this->cbuffer_.world_ = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);
		this->cbuffer_.view_ = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 30.f, -30.f, 0), DirectX::XMVectorZero(), DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f));
		this->cbuffer_.proj_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 16.f / 9.f, 0.1f, 1000.f);
		this->cbuffer2_.viewport_ = DirectX::XMFLOAT2(1, 1);

		this->renderer_       = std::make_shared<Seed::Renderer>();
		this->deffered_       = std::make_shared<Seed::Renderer>();

		this->renderer_->set_constant_buffer(&this->cbuffer_);
		this->deffered_->set_constant_buffer(&this->cbuffer2_);

		this->renderer_->set_rasterizer_state(RS_CW);
		this->renderer_->set_render_targets({ RT_COLOR_MAP, RT_POSITION_MAP, RT_NORMAL_MAP, RT_DEPTH_MAP });
		this->renderer_->set_depth_stencil(DS_DEFFERED);
		this->renderer_->set_shader_file("simple3d-deffered");

		this->deffered_->set_rasterizer_state(RS_CW);
		this->deffered_->set_render_targets({ RT_BACKBUFFER });
		this->deffered_->set_depth_stencil(DS_SIMPLE);
		this->deffered_->set_setup_textures({ RT_NORMAL_MAP });
		this->deffered_->set_shader_file("simple2d-backbuffer");
		this->deffered_->set_model_file("quad2d");

		auto & graphics = this->entity().lock()->scene().lock()->graphics();

		graphics->AddRendererToRenderingList(this->renderer_);
		graphics->AddRendererToRenderingList(this->deffered_);
	}
	void Update(void) override
	{
		this->cbuffer_.world_ *= DirectX::XMMatrixRotationY(.01f);
	}
};