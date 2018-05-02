#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
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

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

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

		ID3DBlob * blob = nullptr;
		ID3DBlob * error = nullptr;

		DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
		shader_flags |= D3DCOMPILE_DEBUG;
#else
		shader_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		if (FAILED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", shader_flags, 0, &blob, &error)))
		{
			if (error != nullptr)
				printf((char*)error->GetBufferPointer());
		}
		else
		{
			this->device_->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->vs().GetAddressOf());
			this->CreateInputLayoutAndConstantBufferFromShader(shader->input_layout().GetAddressOf(), blob, shader->constant_buffer().GetAddressOf());
		}

		if (SUCCEEDED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_0", shader_flags, 0, &blob, &error)))
		{
			this->device_->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->gs().GetAddressOf());
		}

		if (SUCCEEDED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", shader_flags, 0, &blob, &error)))
		{
			this->device_->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->ps().GetAddressOf());
		}

		return shader;
	}

private:
	void CreateInputLayoutAndConstantBufferFromShader(ID3D11InputLayout ** layout, ID3DBlob * blob, ID3D11Buffer ** cbuffer)
	{
		ID3D11ShaderReflection * reflector = nullptr;
		D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);

		// コンスタントバッファーの作成
		D3D11_SHADER_DESC shader_desc;
		reflector->GetDesc(&shader_desc);

		int size = 0;
		if (shader_desc.ConstantBuffers)
		{
			auto cb = reflector->GetConstantBufferByName("unique");
			D3D11_SHADER_BUFFER_DESC desc;
			cb->GetDesc(&desc);

			for (size_t j = 0; j < desc.Variables; ++j) {
				auto v = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC vdesc;
				v->GetDesc(&vdesc);
				size += vdesc.Size;
			}
		}
		//

		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = size;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		bd.Usage = D3D11_USAGE_DYNAMIC;

		if (FAILED(this->device_->CreateBuffer(&bd, nullptr, cbuffer)))
			printf("コンスタントバッファーの作成に失敗しました。");
		//

		std::vector<D3D11_INPUT_ELEMENT_DESC> vbElement;
		for (unsigned int i = 0; i < shader_desc.InputParameters; ++i) {
			D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
			reflector->GetInputParameterDesc(i, &sigdesc);

			auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

			D3D11_INPUT_ELEMENT_DESC eledesc = {
				sigdesc.SemanticName
				, sigdesc.SemanticIndex
				, format
				, 0
				, D3D11_APPEND_ALIGNED_ELEMENT
				, D3D11_INPUT_PER_VERTEX_DATA
				, 0
			};
			vbElement.push_back(eledesc);
		}
		if (!vbElement.empty()) {
			if (FAILED(this->device_->CreateInputLayout(&vbElement[0], vbElement.size(),
				blob->GetBufferPointer(), blob->GetBufferSize(), layout))) {
				printf("インプットレイアウトの作成に失敗しました。");
			};
		}
	}
	DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask)
	{
		if (mask == 0x0F)
		{
			// xyzw
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}

		if (mask == 0x07)
		{
			// xyz
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			}
		}

		if (mask == 0x3)
		{
			// xy
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32_FLOAT;
			}
		}

		if (mask == 0x1)
		{
			// x
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32_FLOAT;
			}
		}

		return DXGI_FORMAT_UNKNOWN;
	}
};