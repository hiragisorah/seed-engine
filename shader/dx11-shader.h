#pragma once

#include <d3d11.h>

#include <seed-engine\shader.h>

class Dx11Shader : public Seed::Shader
{
private:
	ID3D11Buffer * constant_buffer_ = nullptr;
	ID3D11InputLayout * input_layout_ = nullptr;
};