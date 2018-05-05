#pragma once

#include <d3d11.h>
#include <wrl\client.h>

#include <seed-engine\geometry.h>

class Dx11Geometry : public Seed::Geometry
{
public:
	enum DrawMode
	{
		DM_DEFAULT = 0,
		DM_INDEXED,
		DM_INSTANCED,
		DM_INSTANCED_INDEXED
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
	D3D11_PRIMITIVE_TOPOLOGY topology_ = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	unsigned int vertex_size_ = 0;
	unsigned int vertex_num_ = 0;
	unsigned int index_num_ = 0;
	DrawMode draw_mode_ = DM_DEFAULT;

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer(void) const { return this->vertex_buffer_; }
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer(void) const { return this->index_buffer_; }
	const unsigned int vertex_size(void) const { return this->vertex_size_; }
	const D3D11_PRIMITIVE_TOPOLOGY topology(void) const { return this->topology_; }
	const unsigned int vertex_num(void) const { return this->vertex_num_; }
	const unsigned int index_num(void) const { return this->index_num_; }
	const DrawMode draw_mode(void) const { return this->draw_mode_; }
};