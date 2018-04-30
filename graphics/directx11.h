#pragma once

#include <seed-engine\graphics.h>

class DirectX11 : public Seed::Graphics
{
public:
	DirectX11(const std::unique_ptr<Seed::Window> & window)
		: Graphics(window) {}
};