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
		uint32_t material_index_;

		std::vector<float> verts_;
		std::vector<float> texcoords_;
		std::vector<float> tangents_;
		std::vector<float> bitangents_;

		std::vector<float> normals_;
	};

	struct Scene
	{
		std::vector<Mesh> meshes_;
		std::vector<std::string> materials_;
	};

	struct Bounding_box
	{
		float min_x_ = 0;
		float max_x_ = 0;
		
		float min_y_ = 0;
		float max_y_ = 0;
		
		float min_z_ = 0;
		float max_z_ = 0;

		QVector3D calculate_centroid();
	};

	Scene read_scene(const char * uri);

	class Not_compatible_image_type_error : public std::exception
	{
	public:
		Not_compatible_image_type_error(const char* msg)
			: std::exception(msg) {}
	};

	Bounding_box calc_bbox(const Mesh& mesh);

	Bounding_box calc_bbox(const Scene& scene);

	struct Targa_image
	{
		uint16_t width_ = 0;
		uint16_t height_ = 0;
		std::vector<char> data_;
	};

	/**
	Qt が上手くグレイスケール Tga を読み込んでくれないので、
	グレイスケールのみ読み込む Tga パーサー。
	グレイスケールでないとき、Not_compatible_image_type_error を送出する。

	@throws  Not_compatible_image_type_error
	*/
	Targa_image read_targa(std::ifstream& ifst);

	QImage* read_targa(const char* targa_path);
}
