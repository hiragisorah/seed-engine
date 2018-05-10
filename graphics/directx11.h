#pragma once

#include <memory>
#include <string>
#include <vector>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl\client.h>

#include <seed-engine\graphics.h>

#include "directx11\render-target.h"
#include "directx11\rasterizer-state.h"
#include "directx11\model.h"
#include "directx11\shader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTex.lib")

class DirectX11 : public Seed::Graphics
{
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;

public:
	void Initialize(void) override
	{
		// デバイスとスワップチェーンの作成
		DXGI_SWAP_CHAIN_DESC sd;
		memset(&sd, 0, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = this->window().lock()->width();
		sd.BufferDesc.Height = this->window().lock()->height();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = this->window().lock()->hwnd();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		D3D_FEATURE_LEVEL feature_levels = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL * feature_level = nullptr;

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			0, &feature_levels, 1, D3D11_SDK_VERSION, &sd, &this->swap_chain_, &this->device_,
			feature_level, &this->context_);

		this->set_render_target<Dx11RenderTarget>(this->device_, this->context_, this->swap_chain_, this->window());
		this->set_rasterizer_state<Dx11RasterizerState>(this->device_, this->context_);
		this->set_shader<Dx11Shader>(this->device_, this->context_);
		this->set_model<Dx11Model>(this->device_, this->context_);
	};
	void Finalize(void) override {}
	void Present(void) override
	{
		this->swap_chain_->Present(1, 0);
	}
};