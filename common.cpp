#include "stdafx.h"
#include "common.h"

namespace ku {

	Scene read_scene(const char * uri)
	{
		using namespace Assimp;
		using namespace ku;

		QFile f{ uri };
		f.open(QIODevice::ReadOnly);
		const QByteArray& data = f.readAll();
		f.close();

		Importer importer;
		const aiScene* scene = importer.ReadFileFromMemory(data.data(), data.size(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType
		);

		if (!scene) {
			throw std::exception();
		}
		{
			using namespace ku;
			Scene out_scene;

			for (int m = 0; m < scene->mNumMaterials; m++) {
				const aiMaterial* mat = scene->mMaterials[m];
				aiString name;
				mat->Get(AI_MATKEY_NAME, name);

				out_scene.materials_.push_back(name.C_Str());
			}

			for (int m = 0; m < scene->mNumMeshes; m++) {
				const aiMesh* mesh = scene->mMeshes[m];

				Mesh out_mesh;

				out_mesh.material_index_ = mesh->mMaterialIndex;

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