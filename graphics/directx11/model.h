#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wrl\client.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <seed-engine\model.h>

class Dx11Model : public Seed::Model
{
	struct MeshData
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
		unsigned int vertex_size_;
		unsigned int vertex_num_;
	};

	struct ModelData
	{
		std::vector<MeshData> meshes_;
		D3D11_PRIMITIVE_TOPOLOGY topology_;
	};

public:
	Dx11Model(const Microsoft::WRL::ComPtr<ID3D11Device> & device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context)
		: device_(device)
		, context_(context)
	{}
	
private:
	const Microsoft::WRL::ComPtr<ID3D11Device> & device_;
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context_;

private:
	std::unordered_map<std::string, std::unique_ptr<ModelData>> model_list_;

private:
	void Load(std::string name) override
	{
		auto path = "resource/model/" + name + ".sem";
		auto & model = this->model_list_[name] = std::make_unique<ModelData>();

		struct Vertex
		{
			DirectX::XMFLOAT3 position_;
			DirectX::XMFLOAT3 normal_;
			DirectX::XMFLOAT2 uv_;
		};

		struct Mesh
		{
			std::vector<Vertex> vertices_;
		};

		std::vector<Mesh> meshes;

		std::ifstream file;
		file.open(path, std::ios::in | std::ios::binary);

		if (file.fail())
		{
			std::cout << __FUNCTION__ << " ƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s‚µ‚Ü‚µ‚½" << std::endl;
			return;
		}

		unsigned int mesh_cnt = 0;
		file >> mesh_cnt;

		meshes.resize(mesh_cnt);

		for (unsigned int n = 0; n < mesh_cnt; ++n)
		{
			unsigned int vtx_cnt = 0;
			file.read((char*)&vtx_cnt, sizeof(unsigned int));

			meshes[n].vertices_.resize(vtx_cnt);
			for (unsigned int i = 0; i < vtx_cnt; ++i)
			{
				file.read((char*)&meshes[n].vertices_[i], sizeof(Vertex));
			}
		}

		file.close();

		model->meshes_.resize(mesh_cnt);

		for (unsigned int n = 0; n < mesh_cnt; ++n)
		{
			this->CreateVertexBuffer(model->meshes_[n], meshes[n].vertices_);
		}
	}
	void Unload(std::string name) override
	{
		this->model_list_.erase(name);
	}
	void Draw(std::string name) override
	{
		auto & model = this->model_list_[name];

		if (!model) return;

		unsigned int offset = 0;

		this->context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto & mesh : model->meshes_)
		{
			this->context_->IASetVertexBuffers(0, 1, mesh.vertex_buffer_.GetAddressOf(), &mesh.vertex_size_, &offset);
			this->context_->Draw(mesh.vertex_num_, 0);
		}
	}

private:
	template<class _Vertex> void CreateVertexBuffer(MeshData & mesh, std::vector<_Vertex> & vertices)
	{
		mesh.vertex_num_ = vertices.size();
		mesh.vertex_size_ = sizeof(_Vertex);

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = mesh.vertex_num_ * mesh.vertex_size_;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();

		this->device_->CreateBuffer(&bd, &sd, mesh.vertex_buffer_.GetAddressOf());
	}
	template<class _Index> void CreateIndexBuffer(MeshData & mesh, std::vector<_Index> & indices)
	{
		mesh.index_num_ = indices.size();

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = mesh.index_num_ * sizeof(_Index);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = indices.data();

		this->device_->CreateBuffer(&bd, &sd, mesh.index_buffer_.GetAddressOf());
	}
};