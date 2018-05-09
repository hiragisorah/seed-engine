#pragma once

#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include <d3d11.h>
#include <wrl\client.h>

#include <seed-engine\window.h>
#include <seed-engine\render-target.h>

using namespace Seed;

struct RTVAndSRV
{
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
};

class Dx11RenderTarget : public Seed::RenderTarget
{
public:
	Dx11RenderTarget(const Microsoft::WRL::ComPtr<ID3D11Device> & device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context,
		const Microsoft::WRL::ComPtr<IDXGISwapChain> & swap_chain, const std::unique_ptr<Window> & window)
		: device_(device)
		, context_(context)
		, swap_chain_(swap_chain)
		, window_(window)
	{}

private:
	const Microsoft::WRL::ComPtr<ID3D11Device> & device_;
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context_;
	const Microsoft::WRL::ComPtr<IDXGISwapChain> & swap_chain_;
	const std::unique_ptr<Window> & window_;

private:
	D3D11_VIEWPORT vp_[VP_NUM] = {};
	RTVAndSRV rtv_and_srv_[RT_NUM] = {};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> deptsh_stencil_[DS_NUM];

private:
	void create_simple_viewport(void) override
	{
		auto & vp = this->vp_[DS_SIMPLE];

		vp.Width = this->window_->width<float>();
		vp.Height = this->window_->height<float>();
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		vp.TopLeftX = 0.f;
		vp.TopLeftY = 0.f;
	}
	void create_shadow_map_viewport(void) override
	{
		auto & vp = this->vp_[DS_SHADOW_MAP];

		vp.Width = this->window_->width<float>();
		vp.Height = this->window_->height<float>();
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		vp.TopLeftX = 0.f;
		vp.TopLeftY = 0.f;
	}
	void create_backbuffer(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		// バックバッファーテクスチャーを取得
		this->swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&tex2d);
		// そのテクスチャーに対しレンダーターゲットビュー(RTV)を作成
		this->device_->CreateRenderTargetView(tex2d.Get(), nullptr, this->rtv_and_srv_[RT_BACKBUFFER].rtv_.GetAddressOf());
	}
	void create_color_map(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		D3D11_TEXTURE2D_DESC tex_desc = {};
		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

		memset(&tex_desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		memset(&srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		memset(&rtv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		//カラーマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		rtv_desc.Format = tex_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateRenderTargetView(tex2d.Get(), &rtv_desc, this->rtv_and_srv_[RT_COLOR_MAP].rtv_.GetAddressOf());
		this->device_->CreateShaderResourceView(tex2d.Get(), &srv_desc, this->rtv_and_srv_[RT_COLOR_MAP].srv_.GetAddressOf());
	}
	void create_position_map(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		D3D11_TEXTURE2D_DESC tex_desc = {};
		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

		memset(&tex_desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		memset(&srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		memset(&rtv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		//ポジションマップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		rtv_desc.Format = tex_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateRenderTargetView(tex2d.Get(), &rtv_desc, this->rtv_and_srv_[RT_POSITION_MAP].rtv_.GetAddressOf());
		this->device_->CreateShaderResourceView(tex2d.Get(), &srv_desc, this->rtv_and_srv_[RT_POSITION_MAP].srv_.GetAddressOf());
	}
	void create_normal_map(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		D3D11_TEXTURE2D_DESC tex_desc = {};
		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

		memset(&tex_desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		memset(&srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		memset(&rtv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		//法線マップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		rtv_desc.Format = tex_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateRenderTargetView(tex2d.Get(), &rtv_desc, this->rtv_and_srv_[RT_NORMAL_MAP].rtv_.GetAddressOf());
		this->device_->CreateShaderResourceView(tex2d.Get(), &srv_desc, this->rtv_and_srv_[RT_NORMAL_MAP].srv_.GetAddressOf());
	}
	void create_depth_map(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		D3D11_TEXTURE2D_DESC tex_desc = {};
		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

		memset(&tex_desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		memset(&srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		memset(&rtv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		//深度マップ用テクスチャーとそのレンダーターゲットビュー、シェーダーリソースビューの作成	
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		rtv_desc.Format = tex_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateRenderTargetView(tex2d.Get(), &rtv_desc, this->rtv_and_srv_[RT_DEPTH_MAP].rtv_.GetAddressOf());
		this->device_->CreateShaderResourceView(tex2d.Get(), &srv_desc, this->rtv_and_srv_[RT_DEPTH_MAP].srv_.GetAddressOf());
	}
	void create_shadow_map(void) override
	{
		
	}
	void create_simple_depth_stencil(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_D32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateDepthStencilView(tex2d.Get(), nullptr, this->deptsh_stencil_[DS_SIMPLE].GetAddressOf());
	}
	void create_deffered_depth_stencil(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_D32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateDepthStencilView(tex2d.Get(), nullptr, this->deptsh_stencil_[DS_DEFFERED].GetAddressOf());
	}
	void create_shadow_map_depth_stencil(void) override
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = this->window_->width();
		tex_desc.Height = this->window_->height();
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_D32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		this->device_->CreateTexture2D(&tex_desc, nullptr, tex2d.GetAddressOf());
		this->device_->CreateDepthStencilView(tex2d.Get(), nullptr, this->deptsh_stencil_[DS_SHADOW_MAP].GetAddressOf());
	}

private:
	void Clear(void) override
	{
		float clear_color[] = {.3f, .6f, .9f, 1.f};
		for (int n = 0; n < RT_NUM; ++n)
		{
			this->context_->ClearRenderTargetView(this->rtv_and_srv_[n].rtv_.Get(), clear_color);
		}
		for (int n = 0; n < DS_NUM; ++n)
		{
			this->context_->ClearDepthStencilView(this->deptsh_stencil_[n].Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
		}
	}

private:
	void SetupViewport(const VP & vp) override
	{
		this->context_->RSSetViewports(1, &this->vp_[vp]);
	}
	void SetupRenderTargetsAndDepthStencil(const std::vector<RT> & rts, const DS & ds) override
	{
		ID3D11RenderTargetView * rtvs[8] = {};

		for (unsigned int n = 0; n < rts.size(); ++n)
			rtvs[n] = this->rtv_and_srv_[rts[n]].rtv_.Get();

		this->context_->OMSetRenderTargets(rts.size(), rtvs, this->deptsh_stencil_[ds].Get());
	}
	void SetupTextures(const std::vector<RT> & rts) override
	{
		for (unsigned int n = 0; n < rts.size(); ++n)
		{
			if (rts[n] != RT_NONE)
			{
				this->context_->VSSetShaderResources(n, 1, this->rtv_and_srv_[rts[n]].srv_.GetAddressOf());
				this->context_->GSSetShaderResources(n, 1, this->rtv_and_srv_[rts[n]].srv_.GetAddressOf());
				this->context_->PSSetShaderResources(n, 1, this->rtv_and_srv_[rts[n]].srv_.GetAddressOf());
			}
		}
	}
};