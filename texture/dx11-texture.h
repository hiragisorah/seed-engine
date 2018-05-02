#pragma once

#include <seed-engine\texture.h>

class Dx11Texture : public Seed::Texture
{
private:
	unsigned int width_ = 0;
	unsigned int height_ = 0;

public:
	unsigned int width(void) override { return this->width_; }
	unsigned int height(void)override { return this->height_; }
};