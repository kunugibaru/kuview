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
uniform sampler2D Sampler_Roughness;
uniform sampler2D Sampler_Metallic;

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

#define INVERSED_PI 0.3183
#define PI 3.1415

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
uniform sampler2D Sampler_Roughness;
uniform sampler2D Sampler_Metallic;

uniform vec3 LightPos_World;
uniform vec3 LightColor;
uniform float LightPower;
uniform float LightAmbient;


vec3 srgb_to_linear(vec3 c)
{
	return pow(c, vec3(2.2, 2.2, 2.2));
}

vec3 linear_to_srgb(vec3 c)
{
	return pow(c, vec3(0.4545, 0.4545, 0.4545));
}

float g1(vec3 n, vec3 v, float k)
{
	float n_v = dot(n, v);
	return n_v / n_v * (1-k) + k;
}


float microfacets_specular_brdf(
	vec3 n,
	vec3 l,
	vec3 v,
	vec3 spec, // Ks
	float roughness
)
{
	vec3 h = normalize(v + n);
	float v_h = max(0.001, dot(v, h));
	float n_h = max(0.001, dot(n, h));
	float n_l = max(0.001, dot(n, l));
	float n_v = max(0.001, dot(n, v));

	// Normal Distribution
	float alpha = roughness * roughness;
	float temp = alpha / max(0.0001, ((n_h*n_h) * (alpha*alpha-1) + 1));
	float D = temp * temp * INVERSED_PI;
	
	// Geometric Attenuation
	float k = (roughness + 1) * (roughness + 1) / 8;
	float G = g1(n, l, k) * g1(n, v, k);
	

	// Fresnel
	float F0 = spec.r;
	float F = F0 + (1.0 - F0) * pow(2.0, (-5.55473 * v_h - 6.9816) * v_h);

	return D * G * F / (4 * n_l * n_v);
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
	
	vec3 dielectric_color = vec3(0.1, 0.1,0.1);

	vec3 basecolor = srgb_to_linear(texture(Sampler_Basecolor, uv_Model).rgb);
	
	float metallic = texture(Sampler_Metallic, uv_Model).r;
	
	float roughness = max(0.01, texture(Sampler_Roughness, uv_Model).r);	
	
	vec3 diffuse_color = basecolor * (1.0 - metallic);
	vec3 specular_color = mix(dielectric_color, basecolor, metallic);
	
	vec3 point_to_light = LightPos_World - position_World;
	vec3 camera_pos_World = inverse(V)[3].xyz;
	vec3 point_to_camera = camera_pos_World - position_World;	
	float point_to_light_length = length(LightPos_World - position_World);

	vec3 out_diff = (diffuse_color*(vec3(1.0,1.0,1.0)-specular_color) * INVERSED_PI);
	float sbrdf = microfacets_specular_brdf(
		fixed_normal_World,
		point_to_light,
		point_to_camera,
		specular_color,
		roughness
	);
	
	vec3 out_spec = vec3(sbrdf, sbrdf, sbrdf);
	
	float c = max(dot(fixed_normal_World, point_to_light), 0.0);

	float falloff = 1.0/(1.0+0.1*point_to_light_length*point_to_light_length);

	vec3 l = LightColor * (falloff * PI) * (LightPower * 0.1);
	

	vec3 out_color = linear_to_srgb(c * (out_diff + out_spec) * l);

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
