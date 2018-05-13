#pragma once

#include <memory>

#include <DirectXMath.h>

#include <seed-engine\graphics.h>
#include <seed-engine\component.h>
#include <seed-engine\renderer.h>
#include <seed-engine\entity.h>
#include <seed-engine\scene.h>

using namespace Seed;

class GBuffer final : public Component
{
	struct CBUFFER
	{
		DirectX::XMMATRIX world_;
		CBUFFER(void)
			: world_(DirectX::XMMatrixIdentity())
		{}
	};

private:
	std::shared_ptr<Renderer> col_renderer_;
	std::shared_ptr<Renderer> pos_renderer_;
	std::shared_ptr<Renderer> nor_renderer_;
	std::shared_ptr<Renderer> dep_renderer_;
	CBUFFER col_cbuffer_;
	CBUFFER pos_cbuffer_;
	CBUFFER nor_cbuffer_;
	CBUFFER dep_cbuffer_;

public:
	GBuffer(std::shared_ptr<Component> & self)
		: Component(self)
	{}

public:
	void OnAdd(void) override
	{
		this->col_renderer_ = std::make_shared<Renderer>();
		this->col_renderer_->set_rasterizer_state(RS_CW);
		this->col_renderer_->set_render_targets({ RT_BACKBUFFER });
		this->col_renderer_->set_depth_stencil(DS_SIMPLE);
		this->col_renderer_->set_setup_textures({ RT_COLOR_MAP });
		this->col_renderer_->set_shader_file("simple2d-backbuffer");
		this->col_renderer_->set_model_file("quad2d");
		this->col_renderer_->set_constant_buffer(0, &this->col_cbuffer_);

		this->pos_renderer_ = std::make_shared<Renderer>();
		this->pos_renderer_->set_rasterizer_state(RS_CW);
		this->pos_renderer_->set_render_targets({ RT_BACKBUFFER });
		this->pos_renderer_->set_depth_stencil(DS_SIMPLE);
		this->pos_renderer_->set_setup_textures({ RT_POSITION_MAP });
		this->pos_renderer_->set_shader_file("simple2d-backbuffer");
		this->pos_renderer_->set_model_file("quad2d");
		this->pos_renderer_->set_constant_buffer(0, &this->pos_cbuffer_);

		this->nor_renderer_ = std::make_shared<Renderer>();
		this->nor_renderer_->set_rasterizer_state(RS_CW);
		this->nor_renderer_->set_render_targets({ RT_BACKBUFFER });
		this->nor_renderer_->set_depth_stencil(DS_SIMPLE);
		this->nor_renderer_->set_setup_textures({ RT_NORMAL_MAP });
		this->nor_renderer_->set_shader_file("simple2d-backbuffer");
		this->nor_renderer_->set_model_file("quad2d");
		this->nor_renderer_->set_constant_buffer(0, &this->nor_cbuffer_);

		this->dep_renderer_ = std::make_shared<Renderer>();
		this->dep_renderer_->set_rasterizer_state(RS_CW);
		this->dep_renderer_->set_render_targets({ RT_BACKBUFFER });
		this->dep_renderer_->set_depth_stencil(DS_SIMPLE);
		this->dep_renderer_->set_setup_textures({ RT_DEPTH_MAP });
		this->dep_renderer_->set_shader_file("simple2d-backbuffer");
		this->dep_renderer_->set_model_file("quad2d");
		this->dep_renderer_->set_constant_buffer(0, &this->dep_cbuffer_);

		auto & graphics = this->entity().lock()->scene().lock()->graphics();

		graphics.lock()->AddRendererToRenderingList(this->col_renderer_);
		graphics.lock()->AddRendererToRenderingList(this->pos_renderer_);
		graphics.lock()->AddRendererToRenderingList(this->nor_renderer_);
		graphics.lock()->AddRendererToRenderingList(this->dep_renderer_);

		this->col_cbuffer_.world_ = DirectX::XMMatrixScaling(.5f, .5f, 1.f) * DirectX::XMMatrixTranslation(+.5f, -.5f, 0.f);
		this->pos_cbuffer_.world_ = DirectX::XMMatrixScaling(.5f, .5f, 1.f) * DirectX::XMMatrixTranslation(+.5f, +.5f, 0.f);
		this->nor_cbuffer_.world_ = DirectX::XMMatrixScaling(.5f, .5f, 1.f) * DirectX::XMMatrixTranslation(-.5f, -.5f, 0.f);
		this->dep_cbuffer_.world_ = DirectX::XMMatrixScaling(.5f, .5f, 1.f) * DirectX::XMMatrixTranslation(-.5f, +.5f, 0.f);
	}
	void Update(void) override
	{

	}
};