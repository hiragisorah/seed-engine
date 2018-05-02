#pragma once

#include <d3d11.h>
#include <wrl\client.h>

#include <memory>
#include <string>

#include <DirectXTK\DirectXTex.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\SpriteBatch.h>

#include <seed-engine\graphics.h>
#include <texture\dx11-texture.h>
#include <model\dx11-model.h>
#include <shader\dx11-shader.h>

#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "d3d11.lib")

class DirectX11 : public Seed::Graphics
{
public:
	DirectX11(const std::unique_ptr<Seed::Window> & window)
		: Graphics(window) {}

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_ = nullptr;

public:
	void Initialize(void) override
	{
		// デバイスとスワップチェーンの作成
		DXGI_SWAP_CHAIN_DESC sd;
		memset(&sd, 0, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = this->window()->width();
		sd.BufferDesc.Height = this->window()->height();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = this->window()->hwnd();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		D3D_FEATURE_LEVEL feature_levels = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL * feature_level = nullptr;

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			0, &feature_levels, 1, D3D11_SDK_VERSION, &sd, &this->swap_chain_, &this->device_,
			feature_level, &this->context_);
	}
	bool Run(void) override
	{

		this->swap_chain_->Present(1, 0);

		return true;
	}
	void Finalize(void) override
	{

	}

public:
	const std::shared_ptr<Seed::Texture> CreateTexture(std::string file_path) override
	{
		auto texture = std::make_shared<Dx11Texture>();

		std::wstring w_file_path = std::wstring(file_path.begin(), file_path.end());

		DirectX::CreateWICTextureFromFile(this->device_.Get(), w_file_path.c_str(), nullptr, texture->srv());

		return texture;
	}
	const std::shared_ptr<Seed::Model> CreateModel(std::string file_path) override
	{
		auto model = std::make_shared<Dx11Model>();

		

		return model;
	}
	const std::shared_ptr<Seed::Shader> CreateShader(std::string file_path) override
	{
		auto shader = std::make_shared<Dx11Shader>();

		return shader;
	}
};