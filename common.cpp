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

	Bounding_box calc_bbox(const Mesh & mesh)
	{
		Bounding_box out_bbox;

		const uint32_t verts_size = mesh.verts_.size() / 3;
		for (size_t i = 0; i < verts_size; i++) {
			const float x = mesh.verts_.at(i * 3);
			const float y = mesh.verts_.at(i * 3 + 1);
			const float z = mesh.verts_.at(i * 3 + 2);

			if (x > out_bbox.max_x_) {
				out_bbox.max_x_ = x;
			}
			else if (x < out_bbox.min_x_) {
				out_bbox.min_x_ = x;
			}

			if (y > out_bbox.max_y_) {
				out_bbox.max_y_ = y;
			}
			else if (y < out_bbox.min_y_) {
				out_bbox.min_y_ = y;
			}

			if (z > out_bbox.max_z_) {
				out_bbox.max_z_ = z;
			}
			else if (z < out_bbox.min_z_) {
				out_bbox.min_z_ = z;
			}
		}

		return out_bbox;
	}

	Bounding_box calc_bbox(const Scene & scene)
	{
		Bounding_box out_bbox;
		for (const auto& mesh : scene.meshes_) {
			const auto& bbox = calc_bbox(mesh);

			if (bbox.max_x_ > out_bbox.max_x_) {
				out_bbox.max_x_ = bbox.max_x_;
			}
			if (bbox.min_x_ < out_bbox.min_x_) {
				out_bbox.min_x_ = bbox.min_x_;
			}

			if (bbox.max_y_ > out_bbox.max_y_) {
				out_bbox.max_y_ = bbox.max_y_;
			}
			if (bbox.max_y_ < out_bbox.min_y_) {
				out_bbox.min_y_ = bbox.min_y_;
			}

			if (bbox.max_z_ > out_bbox.max_z_) {
				out_bbox.max_z_ = bbox.max_z_;
			}
			if (bbox.min_z_ < out_bbox.min_z_) {
				out_bbox.min_z_ = bbox.min_z_;
			}
		}
		return out_bbox;
	}

	/**
	Qt が上手くグレイスケール Tga を読み込んでくれないので、
	グレイスケールのみ読み込む Tga パーサー。
	グレイスケールでないとき、Not_compatible_image_type_error を送出する。

	@throws  Not_compatible_image_type_error
	*/
	Targa_image read_targa(std::ifstream & ifst)
	{
		char id_filed_length = 0;
		ifst.read(&id_filed_length, 1);

		ifst.seekg(2);
		char image_type = 0;
		ifst.read(&image_type, 1);

		if (static_cast<int>(image_type) != 3) {
			// grayscale のみサポートする。
			throw Not_compatible_image_type_error("Image type not supported");
		}

		ifst.seekg(12);

		Targa_image tga;
		ifst.read(reinterpret_cast<char*>(&tga.width_), 2);
		ifst.read(reinterpret_cast<char*>(&tga.height_), 2);

		ifst.seekg(18 + id_filed_length);

		const auto datasize = tga.width_ * tga.height_;

		tga.data_.resize(datasize);
		ifst.read(tga.data_.data(), datasize);
		ifst.seekg(int(ifst.tellg()) + 8);

		static const char* kTruevisionXFile = "TRUEVISION-XFILE";
		char buff[sizeof(kTruevisionXFile)];

		ifst.read(buff, sizeof(kTruevisionXFile));

		if (strstr(buff, kTruevisionXFile) != 0) {
			throw Not_compatible_image_type_error("Image datasize ");
		}

		return tga;
	}

	QImage * read_targa(const char * targa_path)
	{
		const auto& tga = read_targa(std::ifstream(targa_path, std::ios::binary));

		QImage* img = new QImage(
			(const uchar*)tga.data_.data(),
			tga.width_,
			tga.height_,
			QImage::Format_Grayscale8
		);


		return img;
	}

	QVector3D Bounding_box::calculate_centroid()
	{
		return QVector3D(
			(max_x_ - min_x_) * 0.5f + min_x_,
			(max_y_ - min_y_) * 0.5f + min_y_,
			(max_z_ - min_z_) * 0.5f + min_z_
		);
	}
}