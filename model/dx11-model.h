#pragma once

#include <seed-engine\model.h>

class Dx11Model : public Seed::Model
{
private:
	ID3D11Buffer * vertex_buffer_;
	ID3D11Buffer * index_buffer_;
};