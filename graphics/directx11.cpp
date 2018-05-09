//#include "directx11.h"
//
//#include "directx11\render-target.h"
//#include "directx11\rasterizer-state.h"
//#include "directx11\model.h"
//#include "directx11\shader.h"
//
//void DirectX11::Initialize(void)
//{
//	// デバイスとスワップチェーンの作成
//	DXGI_SWAP_CHAIN_DESC sd;
//	memset(&sd, 0, sizeof(sd));
//	sd.BufferCount = 1;
//	sd.BufferDesc.Width = this->window()->width();
//	sd.BufferDesc.Height = this->window()->height();
//	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	sd.BufferDesc.RefreshRate.Numerator = 60;
//	sd.BufferDesc.RefreshRate.Denominator = 1;
//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	sd.OutputWindow = this->window()->hwnd();
//	sd.SampleDesc.Count = 1;
//	sd.SampleDesc.Quality = 0;
//	sd.Windowed = true;
//
//	D3D_FEATURE_LEVEL feature_levels = D3D_FEATURE_LEVEL_11_0;
//	D3D_FEATURE_LEVEL * feature_level = nullptr;
//
//	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
//		0, &feature_levels, 1, D3D11_SDK_VERSION, &sd, &this->swap_chain_, &this->device_,
//		feature_level, &this->context_);
//
//	this->set_render_target<Dx11RenderTarget>(this->device_, this->context_, this->swap_chain_, this->window());
//	this->set_rasterizer_state<Dx11RasterizerState>(this->device_, this->context_);
//	this->set_shader<Dx11Shader>(this->device_, this->context_);
//	this->set_model<Dx11Model>(this->device_, this->context_);
//}
//
//void DirectX11::Finalize(void)
//{
//}
//
//void DirectX11::Present(void)
//{
//	this->swap_chain_->Present(1, 0);
//}
//
////const std::shared_ptr<Seed::Texture> DirectX11::CreateTexture(std::string file_path)
////{
////	auto texture = std::make_shared<Dx11Texture>();
////
////	std::wstring w_file_path = std::wstring(file_path.begin(), file_path.end());
////
////	DirectX::CreateWICTextureFromFile(this->device_.Get(), w_file_path.c_str(), nullptr, texture->srv().GetAddressOf());
////
////	return texture;
////}