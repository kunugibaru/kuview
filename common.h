#pragma once
#include "stdafx.h"

#ifdef _DEBUG
#pragma comment(lib, "D:/cpp/libs/assimp.lib")

#else

#pragma comment(lib, "D:/cpp/libs/release/assimp.lib")
#endif

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

namespace ku
{

	struct Mesh
	{
		std::vector<uint32_t> faces_;
		std::vector<float> verts_;
		std::vector<float> texcoords_;
		std::vector<float> tangents_;
		std::vector<float> bitangents_;

		std::vector<float> normals_;
	};

	struct Scene
	{
		std::vector<Mesh> meshes_;
	};

	static Scene read_scene(const char* uri)
	{
		using namespace Assimp;

		Importer importer;
		const aiScene* scene = importer.ReadFile(uri,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType
		);

		if (!scene) {
			throw std::exception();
		}

		{

			Scene out_scene;

			for (int m = 0; m < scene->mNumMeshes; m++) {
				const aiMesh* mesh = scene->mMeshes[m];

				Mesh out_mesh;

				for (int f = 0; f < mesh->mNumFaces; f++) {
					const auto& face = mesh->mFaces[f];

					for (int fi = 0; fi < 3; fi++) {
						out_mesh.faces_.push_back(face.mIndices[fi]);
					}
				}

				for (int v = 0; v < mesh->mNumVertices; v++) {
					auto vec = mesh->mVertices[v];
					out_mesh.verts_.push_back(vec.x);
					out_mesh.verts_.push_back(vec.y);
					out_mesh.verts_.push_back(vec.z);

					vec = mesh->mTangents[v];
					out_mesh.tangents_.push_back(vec.x);
					out_mesh.tangents_.push_back(vec.y);
					out_mesh.tangents_.push_back(vec.z);

					vec = mesh->mNormals[v];
					out_mesh.normals_.push_back(vec.x);
					out_mesh.normals_.push_back(vec.y);
					out_mesh.normals_.push_back(vec.z);

					vec = mesh->mBitangents[v];
					out_mesh.bitangents_.push_back(vec.x);
					out_mesh.bitangents_.push_back(vec.y);
					out_mesh.bitangents_.push_back(vec.z);

					const auto ve = mesh->mTextureCoords[0][v];
					out_mesh.texcoords_.push_back(ve.x);
					out_mesh.texcoords_.push_back(ve.y);



				}

				out_scene.meshes_.push_back(out_mesh);
			}

			return out_scene;
		}
	}
}
