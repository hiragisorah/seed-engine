#pragma once

#include <memory>

#include <seed-engine\graphics.h>
#include <texture\dx11-texture.h>

class DirectX11 : public Seed::Graphics
{
public:
	DirectX11(const std::unique_ptr<Seed::Window> & window)
		: Graphics(window) {}

public:
	virtual void Initialize(void) {}
	virtual bool Run(void) { return true; }
	virtual void Finalize(void) {}

public:
	const std::shared_ptr<Seed::Texture> CreateTexture(std::string file_path) override
	{
		auto texture = std::make_shared<Dx11Texture>();

		return texture;
	}
};