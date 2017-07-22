#include "stdafx.h"
#include "Kv_glwidget.h"


const char* basic_vs = R"(#version 410
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

const char* basic_fs = R"(#version 410
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

const char* normal_fs = R"(#version 410
in vec2 uv_Model;

in vec3 normal_Model;
in vec3 bitangent_Model;
in vec3 tangent_Model;

uniform mat4 M;
uniform mat4 V;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform sampler2D Sampler_Basecolor;
uniform sampler2D Sampler_Normal;

vec3 fixNormalSample(vec3 v, bool flipY)
{
	vec3 res = (v - vec3(0.5,0.5,0.5))*2.0;
	res.y = flipY ? -res.y : res.y;
	return res;
}

void main() {
	vec3 normal_Tangent = normalize(texture(Sampler_Normal, uv_Model).xyz);

	vec3 normal_World = (M * vec4(normal_Model, 1.0)).xyz;
	vec3 bitangent_World = (M * vec4(bitangent_Model, 1.0)).xyz;
	vec3 tangent_World = (M * vec4(tangent_Model, 1.0)).xyz;

	vec3 fixed_normal_World = normal_World;

	if (length(normal_Tangent) > 0.0001){
		//normal_Tangent = fixNormalSample(normal_Tangent, false);

		fixed_normal_World = normalize(
		normal_Tangent.x * tangent_World +
		normal_Tangent.y * bitangent_World +
		normal_Tangent.z * normal_World
	);
	}
		

	gl_FragColor = vec4(fixed_normal_World, 1.0);

};
	)";

Kv_glwidget::Kv_glwidget(QWidget* parent = nullptr)
{
	connect(&timer_, SIGNAL(timeout()), this, SLOT(update()));
	timer_.start(16.666f);

	this->setFocusPolicy(Qt::ClickFocus);

	last_mouse_pos_ = this->mapFromGlobal(QCursor::pos());
}


Kv_glwidget::~Kv_glwidget()
{
}

void Kv_glwidget::initializeGL()
{
	this->setUpdatesEnabled(true);

	context_ = QOpenGLContext::currentContext();

	gl = context_->functions();

	gl->glEnable(GL_DEPTH_TEST);

	ku::make_shader(
		program_, basic_vs, basic_fs
	);

	ku::make_shader(
		normalbuffer_program_, basic_vs, normal_fs
	);

	program_.link();
	program_.bind();

	const char* diffusepath = "D:/temps/tex.tga";
	//const char* diffusepath = "D:/temps/uvtemplate.tga";

	gl->glActiveTexture(GL_TEXTURE0);
	texture1_ = new QOpenGLTexture(QImage(diffusepath).mirrored());
	texture1_->bind();
	texture1_->setMagnificationFilter(QOpenGLTexture::Linear);
	texture1_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture1_->setWrapMode(QOpenGLTexture::Repeat);
	gl->glUniform1i(program_.uniformLocation("Sampler_Basecolor"), 0);

	gl->glActiveTexture(GL_TEXTURE1);
	texture2_ = new QOpenGLTexture(QImage("D:/temps/tesnormal.tga").mirrored());
	texture2_->bind();
	texture2_->setMagnificationFilter(QOpenGLTexture::Linear);
	texture2_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture2_->setWrapMode(QOpenGLTexture::Repeat);
	gl->glUniform1i(program_.uniformLocation("Sampler_Normal"), 1);

	swap_model(":/kuview/Resources/machine_01.obj");

	qInfo() << "GL widget initialized";
}

void Kv_glwidget::resizeGL(const int w, const int h)
{
	width_ = w;
	height_ = h;

}

void Kv_glwidget::paintGL()
{
	handle_user_control();

	camera_.update();

	++frame_;

	const auto& set_uniforms = [this](QOpenGLShaderProgram& program) {
		program.bind();
		gl->glUseProgram(program.programId());

		program.setUniformValue("MVP", camera_.mvp());
		program.setUniformValue("M", camera_.model());
		program.setUniformValue("V", camera_.view());

		const QMatrix4x4 VM = camera_.view() * camera_.model();
		program.setUniformValue("NormalMatrix", VM.normalMatrix());

		program.setUniformValue("Frame", frame_);
		point_light_.position_.setX(sin(frame_ * 0.1) * 3);
		point_light_.position_.setZ(cos(frame_ * 0.1) * 3);

		program.setUniformValue("LightPos_World", point_light_.position_);
		program.setUniformValue("LightColor", point_light_.color_);
		program.setUniformValue("LightPower", point_light_.intensity_);
		program.setUniformValue("LightAmbient", point_light_.ambient_);

		gl->glActiveTexture(GL_TEXTURE0);
		texture1_->bind();
		gl->glUniform1i(program.uniformLocation("Sampler_Basecolor"), 0);
		gl->glActiveTexture(GL_TEXTURE1);
		texture2_->bind();
		gl->glUniform1i(program.uniformLocation("Sampler_Normal"), 1);
	};

	for (auto& vattr : this->vert_buffers_) {
		// Clear All
		glViewport(0, 0, width_, height_);
		gl->glClearColor(0.2, 0.2, 0.2, 1);


		// ¶
		glViewport(0, 0, width_ * 0.5, height_);
		vattr->vao_->bind();

		set_uniforms(program_);

		gl->glDrawElements(
			GL_TRIANGLES, scene_.meshes_.front().faces_.size(), GL_UNSIGNED_INT, (void*)0);
		vattr->vao_->release();

		// ‰E
		glViewport(width_ * 0.5, 0, width_ * 0.5, height_);
		set_uniforms(normalbuffer_program_);

		vattr->vao_->bind();
		gl->glDrawElements(
			GL_TRIANGLES, scene_.meshes_.front().faces_.size(), GL_UNSIGNED_INT, (void*)0);

		vattr->vao_->release();
	}


	texture1_->release();
	texture2_->release();
}

ku::Scene Kv_glwidget::swap_model(const char * uri)
{

	scene_ = ku::read_scene(uri);

	this->vert_buffers_.clear();

	/**
	@param(data) data ‚Í std::vector<float> ‚© std::vector<uint32_t>
	*/
	const auto& init_and_set = [](QOpenGLShaderProgram& program,
		QOpenGLBuffer* buffer,
		const GLuint location,
		const auto& data,
		const uint32_t tuplesize
		)
	{
		buffer->create();
		buffer->bind();
		buffer->allocate(data.data(), data.size() * sizeof(float));

		program.enableAttributeArray(location);
		program.setAttributeBuffer(location, GL_FLOAT, 0, tuplesize);
	};

	for (size_t i = 0; i < scene_.meshes_.size(); i++) {
		const auto& mesh = scene_.meshes_.at(i);
		auto& vattr = std::make_unique<Vertex_buffer>();
		vattr->vao_->create();
		vattr->vao_->bind();

		init_and_set(program_, vattr->vert_buff_, 0, mesh.verts_, 3);
		init_and_set(program_, vattr->uv_buff_, 1, mesh.texcoords_, 2);
		init_and_set(program_, vattr->normal_buff_, 2, mesh.normals_, 3);
		init_and_set(program_, vattr->tangent_buff_, 3, mesh.tangents_, 3);
		init_and_set(program_, vattr->bitangent_buff_, 4, mesh.bitangents_, 3);
		init_and_set(program_, vattr->index_buff_, 5, mesh.faces_, 3);

		vattr->vao_->release();

		this->vert_buffers_.push_back(std::move(vattr));
	}

	return scene_;
}

void Kv_glwidget::handle_user_control()
{
	// Key board input
	if (this->last_key_ != Qt::Key::Key_unknown) {

		switch (this->last_key_)
		{
		case Qt::Key_W:
			camera_.translate(camera_.forward());
			break;
		case Qt::Key_A:
			camera_.translate(camera_.left());
			break;
		case Qt::Key_D:
			camera_.translate(camera_.right());
			break;
		case Qt::Key_S:
			camera_.translate(camera_.back());
			break;
		case Qt::Key_F:
			camera_.focus_origin();
			break;
		case Qt::Key_Q:
			camera_.translate(camera_.down());
			break;
		case Qt::Key_E:
			camera_.translate(camera_.up());
			break;
		}

		this->last_key_ = Qt::Key::Key_unknown;
	}

	// Mosue input
	QPoint current_mouse_pos = this->mapFromGlobal(QCursor::pos());

	if (this->is_left_mousebutton_pressed_ == true) {
		QPoint mov = last_mouse_pos_ - current_mouse_pos;

		camera_.rotate_around_target(mov.x());
	}

	last_mouse_pos_ = this->mapFromGlobal(QCursor::pos());
}

void Kv_glwidget::keyPressEvent(QKeyEvent * e)
{
	this->last_key_ = static_cast<Qt::Key>(e->key());
}

void Kv_glwidget::mousePressEvent(QMouseEvent * e)
{
	this->is_left_mousebutton_pressed_ = true;
}

void Kv_glwidget::mouseReleaseEvent(QMouseEvent * e)
{
	this->is_left_mousebutton_pressed_ = false;
}
