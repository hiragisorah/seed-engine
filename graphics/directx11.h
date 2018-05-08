#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl\client.h>

#include <memory>
#include <string>
#include <vector>

#include <DirectXTK\DirectXTex.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>
#include <DirectXTK\Keyboard.h>

#include <seed-engine\graphics.h>
#include <texture\dx11-texture.h>
#include <geometry\dx11-geometry.h>
#include <shader\dx11-shader.h>

#include "directx11\render-target.h"
#include "directx11\rasterizer-state.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

class DirectX11 : public Seed::Graphics
{
public:
	DirectX11(const std::unique_ptr<Seed::Window> & window)
		: Graphics(window) {}

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;

public:
	std::unique_ptr<RasterizerState> rasterizer_state_;

private:
	std::shared_ptr<DirectX::SpriteBatch> sprites_;
	std::shared_ptr<DirectX::SpriteFont> font_;
	std::shared_ptr<Dx11Texture> texture_;

public:
	void Initialize(void) override;
	void Finalize(void) override;
	void Present(void) override;

public:
	const std::shared_ptr<Seed::Texture> CreateTexture(std::string file_path) override;
	const std::shared_ptr<Seed::Geometry> CreateGeometry(std::string file_path) override;
	const std::shared_ptr<Seed::Shader> CreateShader(std::string file_path) override;

public:
	virtual void RenderModel(const std::weak_ptr<Seed::Model> & model) override;

private:
	void CreateInputLayoutAndConstantBufferFromShader(ID3D11InputLayout ** layout, ID3DBlob * blob, ID3D11Buffer ** cbuffer);
	DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask);
};