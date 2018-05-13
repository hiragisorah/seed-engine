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

#if defined(DEBUG) || defined(_DEBUG)
constexpr DWORD SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
constexpr DWORD SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

class Dx11Shader : public Seed::Shader
{
	struct ShaderData
	{
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> constant_buffer_;
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

		if (FAILED(D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", SHADER_FLAGS, 0, &blob, &error)))
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
			this->CreateInputLayoutAndConstantBufferFromShader(shader, blob);
		}

		if (SUCCEEDED(D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_0", SHADER_FLAGS, 0, &blob, &error)))
		{
			this->device_->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->geometry_shader_.GetAddressOf());
		}

		if (SUCCEEDED(D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", SHADER_FLAGS, 0, &blob, &error)))
		{
			this->device_->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->pixel_shader_.GetAddressOf());
		}
	}
	void Unload(std::string name) override
	{
		this->shader_list_.erase(name);
	}
	void Setup(std::string name, const std::vector<void*> & constant_buffers) override
	{
		auto & shader = this->shader_list_[name];

		if (!shader) return;
		
		this->context_->VSSetShader(shader->vertex_shader_.Get(), nullptr, 0);
		this->context_->GSSetShader(shader->geometry_shader_.Get(), nullptr, 0);
		this->context_->PSSetShader(shader->pixel_shader_.Get(), nullptr, 0);

		for (unsigned int n = 0; n < constant_buffers.size(); ++n)
			this->SetConstantBuffer(shader, constant_buffers[n], 0);

		this->context_->IASetInputLayout(shader->input_layout_.Get());
	}

private:
	void CreateInputLayoutAndConstantBufferFromShader(const std::unique_ptr<ShaderData> & shader, ID3DBlob * blob)
	{
		ID3D11ShaderReflection * reflector = nullptr;
		D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);

		D3D11_SHADER_DESC shader_desc;
		reflector->GetDesc(&shader_desc);

		shader->constant_buffer_.resize(shader_desc.ConstantBuffers);

		for (unsigned int n = 0; n < shader_desc.ConstantBuffers; ++n)
		{
			int size = 0;
			auto cb = reflector->GetConstantBufferByIndex(n);
			D3D11_SHADER_BUFFER_DESC desc;
			cb->GetDesc(&desc);

			for (size_t j = 0; j < desc.Variables; ++j)
			{
				auto v = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC vdesc;
				v->GetDesc(&vdesc);
				size += vdesc.Size;
			}

			D3D11_BUFFER_DESC bd;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.ByteWidth = size;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			bd.StructureByteStride = 0;
			bd.Usage = D3D11_USAGE_DEFAULT;

			if (FAILED(this->device_->CreateBuffer(&bd, nullptr, shader->constant_buffer_[n].GetAddressOf())))
				std::cout << "コンスタントバッファーの作成に失敗しました。" << std::endl;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> element;
		for (unsigned int i = 0; i < shader_desc.InputParameters; ++i) {
			D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
			reflector->GetInputParameterDesc(i, &sigdesc);

			auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

			D3D11_INPUT_ELEMENT_DESC eledesc =
			{
				sigdesc.SemanticName
				, sigdesc.SemanticIndex
				, format
				, 0
				, D3D11_APPEND_ALIGNED_ELEMENT
				, D3D11_INPUT_PER_VERTEX_DATA
				, 0
			};

			element.emplace_back(eledesc);
		}

		if (!element.empty())
			if (FAILED(this->device_->CreateInputLayout(&element[0], element.size(),
				blob->GetBufferPointer(), blob->GetBufferSize(), shader->input_layout_.GetAddressOf())))
				std::cout << "インプットレイアウトの作成に失敗しました。" << std::endl;
	}
	void SetConstantBuffer(const std::unique_ptr<ShaderData> & shader, void * constant_buffer, const unsigned int & num)
	{
		this->context_->VSSetConstantBuffers(num, 1, shader->constant_buffer_[num].GetAddressOf());
		this->context_->GSSetConstantBuffers(num, 1, shader->constant_buffer_[num].GetAddressOf());
		this->context_->PSSetConstantBuffers(num, 1, shader->constant_buffer_[num].GetAddressOf());

		this->context_->UpdateSubresource(shader->constant_buffer_[num].Get(), 0, nullptr, constant_buffer, 0, 0);
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