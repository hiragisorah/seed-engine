#pragma once

#include <wrl\client.h>

#include <seed-engine\texture.h>

class Dx11Texture : public Seed::Texture
{
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_ = nullptr;

public:
	ID3D11ShaderResourceView ** const srv(void) { return this->srv_.GetAddressOf(); }

private:
	unsigned int width_ = 0;
	unsigned int height_ = 0;

public:
	unsigned int width(void) override { return this->width_; }
	unsigned int height(void)override { return this->height_; }
};