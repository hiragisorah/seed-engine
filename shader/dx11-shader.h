#pragma once

#include <d3d11.h>

#include <seed-engine\shader.h>

class Dx11Shader : public Seed::Shader
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_ = nullptr;

public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> & vs(void) { return this->vs_; }
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> & gs(void) { return this->gs_; }
	Microsoft::WRL::ComPtr<ID3D11PixelShader> & ps(void) { return this->ps_; }
	Microsoft::WRL::ComPtr<ID3D11Buffer> & constant_buffer(void) { return this->constant_buffer_; }
	Microsoft::WRL::ComPtr<ID3D11InputLayout> & input_layout(void) { return this->input_layout_; }
};