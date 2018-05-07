#include <fstream>

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

	this->CreateBackBuffer();
	this->CreateDeffered();

	this->sprites_ = std::make_shared<DirectX::SpriteBatch>(this->context_.Get());
	this->texture_ = std::static_pointer_cast<Dx11Texture>(this->CreateTexture("resource/texture/grid.bmp"));
	this->font_ = std::make_shared<DirectX::SpriteFont>(this->device_.Get(), L"resource/font/myfile.spritefont");
}

bool DirectX11::Run(void)
{
	// Clear all render targets.
	// レンダーターゲットのクリア
	this->Clear();

	// Target: Deffered
	// ターゲットを設定：ディファード
	this->SetRenderTarget(this->deffered_, 5);
	this->SetTexturesFromRenderTarget(this->deffered_);

	// Target: BackBuffer
	// ターゲットを設定：バックバッファー
	this->SetRenderTarget(this->back_buffer_, 1);
	this->SetTexturesFromRenderTarget(this->deffered_);

	
	for (auto && model : this->rendering_list_[0])
	{
		this->RenderModel(model);
	}

	this->rendering_list_[0].clear();
	//this->sprites_->Begin();

	//this->sprites_->Draw(this->deffered_.srv_[0].Get(), DirectX::XMFLOAT2(0.f, 0.f), nullptr, DirectX::Colors::White, 0.f, DirectX::XMFLOAT2(.0f, .0f), DirectX::XMFLOAT2(1.f, 1.f));

	//this->sprites_->End();

	// Show
	// 表示
	this->swap_chain_->Present(1, 0);

	return true;
}

void DirectX11::Finalize(void)
{
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
	
	geometry->set_draw_mode(Dx11Geometry::DrawMode::DM_DEFAULT);
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

void DirectX11::CreateBackBuffer(void)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

	this->back_buffer_.color_ = { .5f, .4f, 1.f, 1.f };

	// バックバッファーテクスチャーを取得
	this->swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&this->back_buffer_.tex_[0]);
	// そのテクスチャーに対しレンダーターゲットビュー(RTV)を作成
	this->device_->CreateRenderTargetView(this->back_buffer_.tex_[0].Get(), nullptr, this->back_buffer_.rtv_[0].GetAddressOf());

	memset(&srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	// デプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = this->window()->width();
	descDepth.Height = this->window()->height();
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	this->device_->CreateTexture2D(&descDepth, nullptr, &this->back_buffer_.ds_tex_);

	// そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
	this->device_->CreateDepthStencilView(this->back_buffer_.ds_tex_.Get(), nullptr, this->back_buffer_.dsv_.GetAddressOf());

	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	this->device_->CreateShaderResourceView(this->back_buffer_.tex_[0].Get(), &srv_desc, this->back_buffer_.srv_[0].GetAddressOf());

	// ビューポートの作成
	this->back_buffer_.vp_.Width = this->window()->width<float>();
	this->back_buffer_.vp_.Height = this->window()->height<float>();
	this->back_buffer_.vp_.MinDepth = 0.0f;
	this->back_buffer_.vp_.MaxDepth = 1.0f;
	this->back_buffer_.vp_.TopLeftX = 0;
	this->back_buffer_.vp_.TopLeftY = 0;
}

void DirectX11::CreateDeffered(void)
{
	this->deffered_.color_ = { 0.8f, 0.4f, 1.f, 1.f };

	D3D11_TEXTURE2D_DESC tex_desc[5] = {};
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc[5] = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc[5] = {};

	this->deffered_.vp_.Width = this->window()->width<float>();
	this->deffered_.vp_.Height = this->window()->height<float>();
	this->deffered_.vp_.MinDepth = 0.0f;
	this->deffered_.vp_.MaxDepth = 1.0f;
	this->deffered_.vp_.TopLeftX = 0;
	this->deffered_.vp_.TopLeftY = 0;

	//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC ds_tex_desc = {};
	ds_tex_desc.Width = this->window()->width();
	ds_tex_desc.Height = this->window()->height();
	ds_tex_desc.MipLevels = 1;
	ds_tex_desc.ArraySize = 1;
	ds_tex_desc.Format = DXGI_FORMAT_D32_FLOAT;
	ds_tex_desc.SampleDesc.Count = 1;
	ds_tex_desc.SampleDesc.Quality = 0;
	ds_tex_desc.Usage = D3D11_USAGE_DEFAULT;
	ds_tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ds_tex_desc.CPUAccessFlags = 0;
	ds_tex_desc.MiscFlags = 0;

	//Deferred 関連 なおそれぞれ深度ステンシルを作るわけではない。サイズが同じなので通常のものを使い回せる。

	memset(&tex_desc[0], 0, sizeof(D3D11_TEXTURE2D_DESC));
	memset(&srv_desc[0], 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	memset(&rtv_desc[0], 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	//カラーマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
	tex_desc[0].Width = this->window()->width();
	tex_desc[0].Height = this->window()->height();
	tex_desc[0].MipLevels = 1;
	tex_desc[0].ArraySize = 1;
	tex_desc[0].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc[0].SampleDesc.Count = 1;
	tex_desc[0].SampleDesc.Quality = 0;
	tex_desc[0].Usage = D3D11_USAGE_DEFAULT;
	tex_desc[0].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex_desc[0].CPUAccessFlags = 0;
	tex_desc[0].MiscFlags = 0;

	rtv_desc[0].Format = tex_desc[0].Format;
	rtv_desc[0].ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc[0].Texture2D.MipSlice = 0;

	srv_desc[0].Format = tex_desc[0].Format;
	srv_desc[0].ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc[0].Texture2D.MipLevels = 1;

	tex_desc[1] = tex_desc[0];
	rtv_desc[1] = rtv_desc[0];
	srv_desc[1] = srv_desc[0];

	//ノーマルマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
	tex_desc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc[1].SampleDesc.Count = 1;
	tex_desc[1].SampleDesc.Quality = 0;
	tex_desc[1].Usage = D3D11_USAGE_DEFAULT;
	tex_desc[1].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	rtv_desc[1].Format = tex_desc[1].Format;
	rtv_desc[1].ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc[1].Texture2D.MipSlice = 0;

	srv_desc[1].Format = tex_desc[1].Format;
	srv_desc[1].ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc[1].Texture2D.MipLevels = 1;

	tex_desc[2] = tex_desc[1];
	rtv_desc[2] = rtv_desc[1];
	srv_desc[2] = srv_desc[1];

	//ポジションマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
	tex_desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc[2].SampleDesc.Count = 1;
	tex_desc[2].SampleDesc.Quality = 0;
	tex_desc[2].Usage = D3D11_USAGE_DEFAULT;
	tex_desc[2].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	rtv_desc[2].Format = tex_desc[2].Format;
	rtv_desc[2].ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc[2].Texture2D.MipSlice = 0;

	srv_desc[2].Format = tex_desc[2].Format;
	srv_desc[2].ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc[2].Texture2D.MipLevels = 1;

	tex_desc[3] = tex_desc[2];
	rtv_desc[3] = rtv_desc[2];
	srv_desc[3] = srv_desc[2];

	//ポジションマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
	tex_desc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc[3].SampleDesc.Count = 1;
	tex_desc[3].SampleDesc.Quality = 0;
	tex_desc[3].Usage = D3D11_USAGE_DEFAULT;
	tex_desc[3].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	rtv_desc[3].Format = tex_desc[3].Format;
	rtv_desc[3].ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc[3].Texture2D.MipSlice = 0;

	srv_desc[3].Format = tex_desc[3].Format;
	srv_desc[3].ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc[3].Texture2D.MipLevels = 1;

	tex_desc[4] = tex_desc[3];
	rtv_desc[4] = rtv_desc[3];
	srv_desc[4] = srv_desc[3];

	//ポジションマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
	tex_desc[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc[4].SampleDesc.Count = 1;
	tex_desc[4].SampleDesc.Quality = 0;
	tex_desc[4].Usage = D3D11_USAGE_DEFAULT;
	tex_desc[4].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	rtv_desc[4].Format = tex_desc[4].Format;
	rtv_desc[4].ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc[4].Texture2D.MipSlice = 0;

	srv_desc[4].Format = tex_desc[4].Format;
	srv_desc[4].ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc[4].Texture2D.MipLevels = 1;

	for (INT n = 0; n < 5; ++n)
	{
		this->device_->CreateTexture2D(&tex_desc[n], nullptr, this->deffered_.tex_[n].GetAddressOf());
		this->device_->CreateRenderTargetView(this->deffered_.tex_[n].Get(), &rtv_desc[n], this->deffered_.rtv_[n].GetAddressOf());
		this->device_->CreateShaderResourceView(this->deffered_.tex_[n].Get(), &srv_desc[n], this->deffered_.srv_[n].GetAddressOf());
	}
	this->device_->CreateTexture2D(&ds_tex_desc, nullptr, this->deffered_.ds_tex_.GetAddressOf());
	this->device_->CreateDepthStencilView(this->deffered_.ds_tex_.Get(), nullptr, this->deffered_.dsv_.GetAddressOf());
}

void DirectX11::AddModelToRenderingList(const std::weak_ptr<Seed::Model> model, int list_num)
{
	this->rendering_list_[list_num].emplace_back(model);
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

	if (geometry->draw_mode() == Dx11Geometry::DrawMode::DM_DEFAULT)
	{
		this->context_->Draw(geometry->vertex_num(), 0);
	}
	else if (geometry->draw_mode() == Dx11Geometry::DrawMode::DM_INDEXED)
	{
		this->context_->IASetIndexBuffer(geometry->index_buffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		this->context_->DrawIndexed(geometry->index_num(), 0, 0);
	}
	else if (geometry->draw_mode() == Dx11Geometry::DrawMode::DM_INSTANCED)
	{

	}
	else if (geometry->draw_mode() == Dx11Geometry::DrawMode::DM_INSTANCED_INDEXED)
	{
		this->context_->IASetIndexBuffer(geometry->index_buffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	}
}

void DirectX11::Clear(void)
{
	this->context_->ClearRenderTargetView(this->deffered_.rtv_[0].Get(), (float*)&this->deffered_.color_);
	this->context_->ClearDepthStencilView(this->deffered_.dsv_.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
	this->context_->ClearRenderTargetView(this->back_buffer_.rtv_[0].Get(), (float*)&this->back_buffer_.color_);
	this->context_->ClearDepthStencilView(this->back_buffer_.dsv_.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
}

void DirectX11::SetRenderTarget(RenderTarget & rt, unsigned int target_num)
{
	this->context_->OMSetRenderTargets(target_num, rt.rtv_[0].GetAddressOf(), rt.dsv_.Get());
	this->context_->RSSetViewports(1, &rt.vp_);
}

void DirectX11::SetTexturesFromRenderTarget(RenderTarget & rt)
{
	this->context_->VSSetShaderResources(1, kRenderTargetCnt, rt.srv_->GetAddressOf());
	this->context_->GSSetShaderResources(1, kRenderTargetCnt, rt.srv_->GetAddressOf());
	this->context_->PSSetShaderResources(1, kRenderTargetCnt, rt.srv_->GetAddressOf());
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