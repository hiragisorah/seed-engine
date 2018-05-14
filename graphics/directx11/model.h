#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wrl\client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXTK\WICTextureLoader.h>

#include <seed-engine\model.h>

class Dx11Model : public Seed::Model
{
	struct MeshData
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
		unsigned int vertex_size_;
		unsigned int vertex_num_;
		unsigned int index_num_;
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
		auto file_path = "resource/model/" + name + ".sem";
		auto & model = this->model_list_[name] = std::make_unique<ModelData>();

		struct Vertex
		{
			DirectX::XMFLOAT3 position_;
			DirectX::XMFLOAT3 normal_;
			DirectX::XMFLOAT2 uv_;
		};

		struct Mesh
		{
			std::string texture_;
			std::vector<Vertex> vertices_;
			std::vector<unsigned int> indices_;
		};

		std::vector<Mesh> meshes;

		meshes.clear();

		std::ifstream file;

		file.open(file_path, std::ios::in | std::ios::binary);

		unsigned int mesh_cnt;

		file.read(reinterpret_cast<char*>(&mesh_cnt), sizeof(unsigned int));

		meshes.resize(mesh_cnt);

		for (unsigned int m = 0; m < mesh_cnt; ++m)
		{
			auto & mesh = meshes[m];

			unsigned int vtx_cnt = 0;

			file.read(reinterpret_cast<char*>(&vtx_cnt), sizeof(unsigned int));

			mesh.vertices_.resize(vtx_cnt);

			for (unsigned int v = 0; v < vtx_cnt; ++v)
			{
				auto & vtx = mesh.vertices_[v];

				file.read(reinterpret_cast<char*>(&vtx), sizeof(Vertex));
			}

			unsigned int index_cnt = 0;

			file.read(reinterpret_cast<char*>(&index_cnt), sizeof(unsigned int));

			mesh.indices_.resize(index_cnt);

			for (unsigned int i = 0; i < index_cnt; ++i)
			{
				auto & index = mesh.indices_[i];

				file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
			}

			unsigned texture_str_cnt = 0;

			file.read(reinterpret_cast<char*>(&texture_str_cnt), sizeof(unsigned int));

			mesh.texture_.resize(texture_str_cnt);

			if (texture_str_cnt > 0)
			{
				char * texture_str = new char[texture_str_cnt + 1];
				file.read(&texture_str[0], sizeof(char) * texture_str_cnt);
				texture_str[texture_str_cnt] = '\0';
				mesh.texture_ = texture_str;
				mesh.texture_ = mesh.texture_.substr(mesh.texture_.rfind("\\") + 1, mesh.texture_.size() - mesh.texture_.rfind("\\"));

				delete[] texture_str;
			}
		}

		file.close();
		for (unsigned int n = 0; n < mesh_cnt; ++n)
		{
			if (meshes[n].texture_ != "")
			{
				std::wstring w_file_path = L"resource/texture/" + std::wstring(meshes[n].texture_.begin(), meshes[n].texture_.end());
				DirectX::CreateWICTextureFromFile(this->device_.Get(), w_file_path.c_str(), nullptr, model->meshes_[n].texture_.GetAddressOf());
			}

			this->CreateVertexBuffer(model->meshes_[n], meshes[n].vertices_);
			this->CreateIndexBuffer(model->meshes_[n], meshes[n].indices_);
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

		this->context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (auto & mesh : model->meshes_)
		{
			this->context_->IASetVertexBuffers(0, 1, mesh.vertex_buffer_.GetAddressOf(), &mesh.vertex_size_, &offset);
			this->context_->IASetIndexBuffer(mesh.index_buffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
			this->context_->DrawIndexed(mesh.index_num_, 0, 0);
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