#pragma once

#include <iostream>
#include <functional>

#include <d3d11.h>
#include <wrl\client.h>

#include <seed-engine\rasterizer-state.h>

using namespace Seed;

class Dx11RasterizerState : public Seed::RasterizerState
{
public:
	Dx11RasterizerState(const Microsoft::WRL::ComPtr<ID3D11Device> & device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context)
		: device_(device)
		, context_(context)
	{
		
	}

private:
	const Microsoft::WRL::ComPtr<ID3D11Device> & device_;
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context_;

private:
	void create_cull_none(void) override
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->rasterizer_states_[RS_CULL_NONE].GetAddressOf());

		if (FAILED(hr))
			std::cout << "Filed on create: RasterizerState - cull_none" << std::endl;
	}
	void create_cull_clock_wise(void) override
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_FRONT;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->rasterizer_states_[RS_CW].GetAddressOf());

		if (FAILED(hr))
			std::cout << "Filed on create: RasterizerState - cull_clock_wise" << std::endl;
	}
	void create_cull_counter_clock_wise(void) override
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->rasterizer_states_[RS_CCW].GetAddressOf());

		if (FAILED(hr))
			std::cout << "Filed on create: RasterizerState - cull_counter_clock_wise" << std::endl;
	}
	void create_wireframe(void) override
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->rasterizer_states_[RS_WIREFRAME].GetAddressOf());

		if (FAILED(hr))
			std::cout << "Filed on create: RasterizerState - wire_frame" << std::endl;
	}

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states_[RS_NUM];

public:
	void Setup(const RS & rasterizer_state) override
	{
		this->context_->RSSetState(this->rasterizer_states_[rasterizer_state].Get());
	}
};