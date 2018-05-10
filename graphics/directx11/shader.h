#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wrl\client.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <seed-engine\shader.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Dx11Shader : public Seed::Shader
{
	struct ShaderData
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
	};

public:
	Dx11Shader(const Microsoft::WRL::ComPtr<ID3D11Device> & device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context)
		: device_(device)
		, context_(context)
	{}

private:
	const Microsoft::WRL::ComPtr<ID3D11Device> & device_;
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context_;

private:
	std::unordered_map<std::string, std::unique_ptr<ShaderData>> shader_list_;

private:
	void Load(std::string name) override
	{
		auto path = "resource/shader/" + name + ".hlsl";
		auto & shader = this->shader_list_[name] = std::make_unique<ShaderData>();

		ID3DBlob * blob = nullptr;
		ID3DBlob * error = nullptr;

		DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
		shader_flags |= D3DCOMPILE_DEBUG;
#else
		shader_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		if (FAILED(D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", shader_flags, 0, &blob, &error)))
		{
			if (error != nullptr)
				std::cout << __FUNCTION__ << " " << (char*)error->GetBufferPointer() << std::endl;
			else
				std::cout << __FUNCTION__ << " シェーダーの読み込みに失敗しました。" << std::endl;

			return;
		}
		else
		{
			this->device_->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->vertex_shader_.GetAddressOf());
			this->CreateInputLayoutAndConstantBufferFromShader(shader->input_layout_.GetAddressOf(), blob, shader->constant_buffer_.GetAddressOf());
		}

		if (SUCCEEDED(D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_0", shader_flags, 0, &blob, &error)))
		{
			this->device_->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->geometry_shader_.GetAddressOf());
		}

		if (SUCCEEDED(D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", shader_flags, 0, &blob, &error)))
		{
			this->device_->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->pixel_shader_.GetAddressOf());
		}
	}
	void Unload(std::string name) override
	{
		this->shader_list_.erase(name);
	}
	void Setup(std::string name, void * constant_buffer) override
	{
		auto & shader = this->shader_list_[name];

		if (!shader) return;
		
		this->context_->VSSetShader(shader->vertex_shader_.Get(), nullptr, 0);
		this->context_->GSSetShader(shader->geometry_shader_.Get(), nullptr, 0);
		this->context_->PSSetShader(shader->pixel_shader_.Get(), nullptr, 0);

		this->context_->VSSetConstantBuffers(0, 1, shader->constant_buffer_.GetAddressOf());
		this->context_->GSSetConstantBuffers(0, 1, shader->constant_buffer_.GetAddressOf());
		this->context_->PSSetConstantBuffers(0, 1, shader->constant_buffer_.GetAddressOf());

		if (shader->constant_buffer_)
			this->context_->UpdateSubresource(shader->constant_buffer_.Get(), 0, nullptr, constant_buffer, 0, 0);

		this->context_->IASetInputLayout(shader->input_layout_.Get());
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
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		bd.Usage = D3D11_USAGE_DEFAULT;

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
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32G32B32A32_UINT;
			}
		}

		if (mask == 0x07)
		{
			// xyz
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32G32B32_UINT;
			}
		}

		if (mask == 0x3)
		{
			// xy
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32_FLOAT;
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32G32_UINT;
			}
		}

		if (mask == 0x1)
		{
			// x
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32_FLOAT;
			case D3D_REGISTER_COMPONENT_UINT32:
				return DXGI_FORMAT_R32_UINT;
			}
		}

		return DXGI_FORMAT_UNKNOWN;
	}
};