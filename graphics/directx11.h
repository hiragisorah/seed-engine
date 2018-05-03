#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl\client.h>

#include <memory>
#include <string>

#include <DirectXTK\DirectXTex.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

#include <seed-engine\graphics.h>
#include <texture\dx11-texture.h>
#include <model\dx11-model.h>
#include <shader\dx11-shader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

constexpr unsigned int kRenderTargetCnt = 10;

class DirectX11 : public Seed::Graphics
{
	struct RenderTarget
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_[kRenderTargetCnt];
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv_[kRenderTargetCnt];
		Microsoft::WRL::ComPtr<ID3D11Texture2D> ds_tex_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_[kRenderTargetCnt];
		DirectX::XMFLOAT4 color_ = { 1.f, 1.f, 1.f, 1.f };
		D3D11_VIEWPORT vp_ = {};
	};

public:
	DirectX11(const std::unique_ptr<Seed::Window> & window)
		: Graphics(window) {}

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_ = nullptr;

private:
	RenderTarget back_buffer_;
	RenderTarget deffered_;

private:
	std::shared_ptr<DirectX::SpriteBatch> sb;
	std::shared_ptr<DirectX::SpriteFont> sf;
	std::shared_ptr<Dx11Texture> texture_;

public:
	void Initialize(void) override;
	bool Run(void) override;
	void Finalize(void) override;

public:
	const std::shared_ptr<Seed::Texture> CreateTexture(std::string file_path) override;
	const std::shared_ptr<Seed::Model> CreateModel(std::string file_path) override;
	const std::shared_ptr<Seed::Shader> CreateShader(std::string file_path) override;

private:
	void CreateBackBuffer(void) override;
	void CreateDeffered(void) override;

private:
	void CreateInputLayoutAndConstantBufferFromShader(ID3D11InputLayout ** layout, ID3DBlob * blob, ID3D11Buffer ** cbuffer);
	DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask);
};