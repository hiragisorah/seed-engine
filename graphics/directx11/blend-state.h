#pragma once

#include <iostream>
#include <functional>

#include <d3d11.h>
#include <wrl\client.h>

class RasterizerState
{
public:
	enum RS
	{
		RS_CULL_NONE = 0,
		RS_CW,
		RS_CCW,
		RS_WIREFRAME,
		RS_NUM
	};

public:
	RasterizerState(const Microsoft::WRL::ComPtr<ID3D11Device> & device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context)
		: device_(device)
		, context_(context)
	{
		this->create_cull_none();
		this->create_cull_clock_wise();
		this->create_cull_counter_clock_wise();
		this->create_wireframe();

		this->setup_functions_[RS_CULL_NONE] = this->setup_cull_none;
		this->setup_functions_[RS_CW] = this->setup_cull_clock_wise;
		this->setup_functions_[RS_CCW] = this->setup_cull_counter_clock_wise;
		this->setup_functions_[RS_WIREFRAME] = this->setup_wireframe;
	}

private:
	const Microsoft::WRL::ComPtr<ID3D11Device> & device_;
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context_;

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cull_none_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cull_clock_wise_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cull_counter_clock_wise_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframe_ = nullptr;

private:
	void create_cull_none(void)
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->cull_none_.GetAddressOf());

		if (SUCCEEDED(hr))
			std::cout << "Filed on create: RasterizerState - cull_none" << std::endl;
	}
	void create_cull_clock_wise(void)
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_FRONT;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->cull_clock_wise_.GetAddressOf());

		if (SUCCEEDED(hr))
			std::cout << "Filed on create: RasterizerState - cull_clock_wise" << std::endl;
	}
	void create_cull_counter_clock_wise(void)
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->cull_counter_clock_wise_.GetAddressOf());

		if (SUCCEEDED(hr))
			std::cout << "Filed on create: RasterizerState - cull_counter_clock_wise" << std::endl;
	}
	void create_wireframe(void)
	{
		D3D11_RASTERIZER_DESC desc = {};

		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		HRESULT hr = this->device_->CreateRasterizerState(&desc, this->wire_frame_.GetAddressOf());

		if (SUCCEEDED(hr))
			std::cout << "Filed on create: RasterizerState - wire_frame" << std::endl;
	}

private:
	std::function<void(void)> setup_functions_[RS_NUM];

public:
	void Setup(const RS & rasterizer_state)
	{
		this->setup_functions_[rasterizer_state]();
	}

public:
	void setup_cull_none(void)
	{
		this->context_->RSSetState(this->cull_none_.Get());
	}
	void setup_cull_clock_wise(void)
	{
		this->context_->RSSetState(this->cull_clock_wise_.Get());
	}
	void setup_cull_counter_clock_wise(void)
	{
		this->context_->RSSetState(this->cull_counter_clock_wise_.Get());
	}
	void setup_wireframe(void)
	{
		this->context_->RSSetState(this->wireframe_.Get());
	}
};