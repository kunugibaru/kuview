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

	Scene read_scene(const char * uri);
}
