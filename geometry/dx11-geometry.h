#pragma once

#include <d3d11.h>
#include <wrl\client.h>

#include <seed-engine\geometry.h>

enum DM
{
	DM_DEFAULT = 0,
	DM_INDEXED,
	DM_INSTANCED,
	DM_INSTANCED_INDEXED
};

class Dx11Geometry : public Seed::Geometry
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
	D3D11_PRIMITIVE_TOPOLOGY topology_ = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	unsigned int vertex_size_ = 0;
	unsigned int vertex_num_ = 0;
	unsigned int index_num_ = 0;
	DM draw_mode_ = DM_DEFAULT;

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer(void) const { return this->vertex_buffer_; }
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer(void) const { return this->index_buffer_; }
	const unsigned int vertex_size(void) const { return this->vertex_size_; }
	const D3D11_PRIMITIVE_TOPOLOGY topology(void) const { return this->topology_; }
	const unsigned int vertex_num(void) const { return this->vertex_num_; }
	const unsigned int index_num(void) const { return this->index_num_; }
	const DM draw_mode(void) const { return this->draw_mode_; }

public:
	void set_draw_mode(const DM & draw_mode) { this->draw_mode_ = draw_mode; }
	void set_topology(const D3D11_PRIMITIVE_TOPOLOGY & topology) { this->topology_ = topology; }

public:
	template<class _Vertex> void CreateVertexBuffer(ID3D11Device * device, std::vector<_Vertex> & vertices)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = vertices.size() * sizeof(_Vertex);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();

		device->CreateBuffer(&bd, &sd, this->vertex_buffer_.GetAddressOf());

		this->vertex_size_ = sizeof(_Vertex);
		this->vertex_num_ = vertices.size();
	}
	template<class _Index> void CreateIndexBuffer(ID3D11Device * device, std::vector<_Index> & indices)
	{
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = indices.size() * sizeof(_Index);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = indices.data();

		device->CreateBuffer(&bd, &sd, this->index_buffer_.GetAddressOf());
	}
};