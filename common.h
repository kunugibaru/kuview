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
	static const char* basic_vs = R"(#version 410
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitangent;

out vec3 normal_Model;
out vec3 bitangent_Model;
out vec3 tangent_Model;

out vec2 uv_Model;
out vec3 position_World;

out vec3 eyedirection_Camera;
out vec3 lightdirection_Camera;

out vec3 lightdirection_Tangent;
out vec3 eyedirection_Tangent;

uniform mat4 M;
uniform mat4 V;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform vec3 LightPos_World;

void main(){
	normal_Model = vertex_normal;
	bitangent_Model = vertex_bitangent;
	tangent_Model = vertex_tangent;

	gl_Position = MVP * vec4(vertex_position, 1);

	position_World = (M * vec4(vertex_position, 1)).xyz;

	vec3 position_Camera = (V * M * vec4(vertex_position, 1)).xyz;
	eyedirection_Camera = vec3(0, 0, 0) - position_Camera;

	vec3 light_position_Camera = (V * vec4(LightPos_World, 1)).xyz;
	lightdirection_Camera = light_position_Camera + eyedirection_Camera;

	uv_Model = vertex_uv;

	vec3 tangent_Camera = NormalMatrix * vertex_tangent;
	vec3 bitangent_Camera = NormalMatrix * vertex_bitangent;
	vec3 normal_Camera = NormalMatrix * vertex_normal;

	mat3 TBN = transpose(mat3(
		tangent_Camera,
		bitangent_Camera,
		normal_Camera
	));

	lightdirection_Tangent = TBN * lightdirection_Camera;
	eyedirection_Tangent = TBN * eyedirection_Camera;
}
)";

	static const char* basic_fs = R"(#version 410
in vec2 uv_Model;
in vec3 position_World;

in vec3 eyedirection_Camera;
in vec3 lightdirection_Camera;

in vec3 lightdirection_Tangent;
in vec3 eyedirection_Tangent;

uniform sampler2D Sampler_Basecolor;
uniform sampler2D Sampler_Normal;

uniform vec3 LightPos_World;
uniform vec3 LightColor;
uniform float LightPower;
uniform float LightAmbient;

layout(location = 0) out vec3 color;

void main()
{
	vec3 material_diffuse_color = texture(Sampler_Basecolor, uv_Model).rgb;
	vec3 material_ambient_color = vec3(LightAmbient, LightAmbient, LightAmbient) * material_diffuse_color;
	vec3 material_specular_color = vec3(0.3, 0.3, 0.3);
	
	vec3 texture_normal_Tangent = normalize(texture(Sampler_Normal, uv_Model).rgb * 2.0 - 1.0);

	float distance = length(LightPos_World - position_World);

	vec3 n = texture_normal_Tangent;
	vec3 l = normalize(lightdirection_Tangent);

	float cosTheta = clamp(dot(n, l), 0, 1);

	vec3 E = normalize(eyedirection_Tangent);

	vec3 R = reflect(-l, n);

	float cosAlpha = clamp(dot(E, R), 0, 1);

	color = 
		material_ambient_color +
		material_diffuse_color * LightColor * LightPower * cosTheta / (distance * distance) +
		material_specular_color * LightColor * LightPower * pow(cosAlpha, 5) / (distance * distance);
}
)";

	static const char* normal_fs = R"(#version 410

#define M_INV_PI 0.31830988618379067153776752674503
#define M_INV_LOG2 1.4426950408889634073599246810019
#define M_PI 3.1415926535897932384626433832795
#define DISTANCE_ATTENUATION_MULT 0.001

in vec3 normal_Model;
in vec3 bitangent_Model;
in vec3 tangent_Model;

in vec2 uv_Model;
in vec3 position_World;

in vec3 eyedirection_Camera;
in vec3 lightdirection_Camera;

in vec3 lightdirection_Tangent;
in vec3 eyedirection_Tangent;

uniform mat4 M;
uniform mat4 V;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform sampler2D Sampler_Basecolor;
uniform sampler2D Sampler_Normal;

uniform vec3 LightPos_World;
uniform vec3 LightColor;
uniform float LightPower;
uniform float LightAmbient;


vec3 srgb_to_linear(vec3 c)
{
	return pow(c, vec3(2.2, 2.2, 2.2));
}

vec3 linerar_to_srgb(vec3 c)
{
	return pow(c, vec3(0.4545, 0.4545, 0.4545));
}

float G1(
	float ndw, // w is either Ln or Vn
	float k)
{
// One generic factor of the geometry function divided by ndw
// NB : We should have k > 0
	return 1.0 / ( ndw*(1.0-k) + k );
}

float visibility(
	float ndl,
	float ndv,
	float Roughness)
{
// Schlick with Smith-like choice of k
// cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
// visibility is a Cook-Torrance geometry function divided by (n.l)*(n.v)
	float k = Roughness * Roughness * 0.5;
	return G1(ndl,k)*G1(ndv,k);
}

float normal_distrib(
	float ndh,
	float Roughness)
{
// use GGX / Trowbridge-Reitz, same as Disney and Unreal 4
// cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
	float alpha = Roughness * Roughness;
	float tmp = alpha / max(1e-8,(ndh*ndh*(alpha*alpha-1.0)+1.0));
	return tmp * tmp * M_INV_PI;
}

vec3 fresnel(
	float vdh,
	vec3 F0)
{
// Schlick with Spherical Gaussian approximation
// cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
	float sphg = pow(2.0, (-5.55473*vdh - 6.98316) * vdh);
	return F0 + (vec3(1.0, 1.0, 1.0) - F0) * sphg;
}

vec3 microfacets_brdf(
	vec3 Nn,
	vec3 Ln,
	vec3 Vn,
	vec3 Ks,
	float Roughness)
{
	vec3 Hn = normalize(Vn + Ln);
	float vdh = max( 0.0, dot(Vn, Hn) );
	float ndh = max( 0.0, dot(Nn, Hn) );
	float ndl = max( 0.0, dot(Nn, Ln) );
	float ndv = max( 0.0, dot(Nn, Vn) );
	return fresnel(vdh,Ks) *
		( normal_distrib(ndh,Roughness) * visibility(ndl,ndv,Roughness) / 4.0 );
}

float lampAttenuation(float distance)
{
	return 1.0/(1.0+DISTANCE_ATTENUATION_MULT*distance*distance);
}

void main() {
	vec3 normal_Tangent = normalize(texture(Sampler_Normal, uv_Model).xyz);

	vec3 normal_World = (M * vec4(normal_Model, 1.0)).xyz;
	vec3 bitangent_World = (M * vec4(bitangent_Model, 1.0)).xyz;
	vec3 tangent_World = (M * vec4(tangent_Model, 1.0)).xyz;

	vec3 fixed_normal_World = normal_World;

	if (length(normal_Tangent) > 0.0001){
		normal_Tangent = (normal_Tangent - vec3(0.5,0.5,0.5))*2.0;

		fixed_normal_World = normalize(
		normal_Tangent.x * tangent_World +
		normal_Tangent.y * bitangent_World +
		normal_Tangent.z * normal_World
	);
	}
	
	vec3 dielectric_color = vec3(1,1,1);

	vec3 basecolor = srgb_to_linear(texture(Sampler_Basecolor, uv_Model).rgb);
	
	float metallic = texture(Sampler_Normal, uv_Model).r;
	float roughness = max(0.01, texture(Sampler_Normal, uv_Model).r);	

	vec3 diffuse_color = basecolor * (1.0 - metallic);
	vec3 specular_color = mix(dielectric_color, basecolor, metallic);

	vec3 point_to_light = LightPos_World - position_World;
	vec3 camera_pos_World = inverse(V)[3].xyz;
	vec3 point_to_camera = camera_pos_World - position_World;	
	float point_to_light_length = length(LightPos_World - position_World);

	vec3 out_diff = (diffuse_color*(vec3(1.0,1.0,1.0)-specular_color) * M_INV_PI);
	vec3 out_spec = microfacets_brdf(
		fixed_normal_World,
		point_to_light,
		point_to_camera,
		specular_color,
		roughness
	);
	
	float c = max(dot(fixed_normal_World, point_to_light), 0.0);

	vec3 l = LightColor * (lampAttenuation(point_to_light_length) * M_PI);

	vec3 out_color = c * (out_diff + out_spec) * l;

	gl_FragColor = vec4(out_color, 1.0);

};
	
	
	
	)";


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
