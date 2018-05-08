#include <fstream>

#include <DirectXTK\CommonStates.h>
#include <DirectXTK\GeometricPrimitive.h>
#include <seed-engine\utils.h>

#include "directx11.h"

struct GVertex
{
	DirectX::XMFLOAT3 position_;
	DirectX::XMFLOAT3 normal_;
	DirectX::XMFLOAT2 uv_;
};

struct GMesh
{
	std::vector<GVertex> vertices_;
};

void DirectX11::Initialize(void)
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

	this->set_render_target<Dx11RenderTarget>(this->device_, this->context_, this->swap_chain_, this->window());
	this->set_rasterizer_state<Dx11RasterizerState>(this->device_, this->context_);

	this->sprites_ = std::make_shared<DirectX::SpriteBatch>(this->context_.Get());
	this->texture_ = std::static_pointer_cast<Dx11Texture>(this->CreateTexture("resource/texture/grid.bmp"));
	this->font_ = std::make_shared<DirectX::SpriteFont>(this->device_.Get(), L"resource/font/myfile.spritefont");
}

void DirectX11::Finalize(void)
{
}

void DirectX11::Present(void)
{
	this->swap_chain_->Present(1, 0);
}

const std::shared_ptr<Seed::Texture> DirectX11::CreateTexture(std::string file_path)
{
	auto texture = std::make_shared<Dx11Texture>();

	std::wstring w_file_path = std::wstring(file_path.begin(), file_path.end());

	DirectX::CreateWICTextureFromFile(this->device_.Get(), w_file_path.c_str(), nullptr, texture->srv().GetAddressOf());

	return texture;
}

const std::shared_ptr<Seed::Geometry> DirectX11::CreateGeometry(std::string file_path)
{
	auto geometry = std::make_shared<Dx11Geometry>();

	std::vector<GMesh> meshes;

	std::ifstream file;
	file.open(file_path, std::ios::in | std::ios::binary);

	if (file.fail()) return geometry;

	unsigned int mesh_cnt = 0;
	file >> mesh_cnt;

	meshes.resize(mesh_cnt);

	for (unsigned int n = 0; n < mesh_cnt; ++n)
	{
		unsigned int vtx_cnt = 0;
		file.read((char*)&vtx_cnt, sizeof(unsigned int));

		meshes[n].vertices_.resize(vtx_cnt);
		for (unsigned int i = 0; i < vtx_cnt; ++i)
		{
			file.read((char*)&meshes[n].vertices_[i], sizeof(GVertex));
		}
	}

	file.close();

	geometry->CreateVertexBuffer(this->device_.Get(), meshes[0].vertices_);
	
	geometry->set_draw_mode(DM_DEFAULT);
	geometry->set_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return geometry;
}

const std::shared_ptr<Seed::Shader> DirectX11::CreateShader(std::string file_path)
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

void DirectX11::RenderModel(const std::weak_ptr<Seed::Model>& model)
{
	auto shader = model.lock()->shader<Dx11Shader>().lock();
	auto geometry = model.lock()->geometry<Dx11Geometry>().lock();

	unsigned int vertex_size = geometry->vertex_size();
	unsigned int offset = 0;

	this->context_->VSSetShader(shader->vs().Get(), nullptr, 0);
	this->context_->GSSetShader(shader->gs().Get(), nullptr, 0);
	this->context_->PSSetShader(shader->ps().Get(), nullptr, 0);

	this->context_->UpdateSubresource(shader->constant_buffer().Get(), 0, nullptr, model.lock()->constant_buffer(), 0, 0);

	this->context_->VSSetConstantBuffers(0, 1, shader->constant_buffer().GetAddressOf());
	this->context_->GSSetConstantBuffers(0, 1, shader->constant_buffer().GetAddressOf());
	this->context_->PSSetConstantBuffers(0, 1, shader->constant_buffer().GetAddressOf());

	this->context_->IASetInputLayout(shader->input_layout().Get());
	this->context_->IASetVertexBuffers(0, 1, geometry->vertex_buffer().GetAddressOf(), &vertex_size, &offset);
	this->context_->IASetPrimitiveTopology(geometry->topology());

	if (geometry->draw_mode() == DM_DEFAULT)
	{
		this->context_->Draw(geometry->vertex_num(), 0);
	}
	else if (geometry->draw_mode() == DM_INDEXED)
	{
		this->context_->IASetIndexBuffer(geometry->index_buffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		this->context_->DrawIndexed(geometry->index_num(), 0, 0);
	}
	else if (geometry->draw_mode() == DM_INSTANCED)
	{

	}
	else if (geometry->draw_mode() == DM_INSTANCED_INDEXED)
	{
		this->context_->IASetIndexBuffer(geometry->index_buffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	}
}

void DirectX11::CreateInputLayoutAndConstantBufferFromShader(ID3D11InputLayout ** layout, ID3DBlob * blob, ID3D11Buffer ** cbuffer)
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

DXGI_FORMAT DirectX11::GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask)
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