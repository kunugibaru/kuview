#pragma once
#include "stdafx.h"
#include "qopenglwidget.h"
#include "Kv_camera.h"
#include "common.h"

namespace ku
{
	class ShaderCompilationError : std::exception
	{
	public:
		ShaderCompilationError() : std::exception("") {}
		ShaderCompilationError(const char* msg) : std::exception(msg) {}
	};

	/**
	tuplesize: num of components per vertex
	*/
	template<typename N = float>
	static void init_and_set_buffer(
		QOpenGLShaderProgram& program,
		QOpenGLBuffer& buffer,
		const GLuint location,
		const std::vector<N>& data,
		const uint32_t tuplesize
	)
	{
		buffer.create();
		buffer.bind();
		buffer.allocate(data.data(), data.size() * sizeof(float));

		program.enableAttributeArray(location);
		program.setAttributeBuffer(location, GL_FLOAT, 0, tuplesize);
	}

	static void make_shader(
		QOpenGLShaderProgram& program,
		const char* vs,
		const char* fs
	)
	{
		bool result = true;
		result = program.addShaderFromSourceCode(QOpenGLShader::Vertex, vs);

		if (!result) {
			throw ShaderCompilationError(program.log().toStdString().c_str());
		}

		result = program.addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
		if (!result) {
			throw ShaderCompilationError(program.log().toStdString().c_str());
		}
		program.link();
	}

	struct Point_light
	{
		QVector3D position_{ 1,5,3 };
		QVector3D color_{ 1,1,1 };
		float intensity_ = 10;
		float ambient_ = 0.1;
	};


}



class Kv_glwidget :
	public QOpenGLWidget
{
public:
	Kv_glwidget(QWidget* parent);
	~Kv_glwidget();

	uint32_t width_ = 0;
	uint32_t height_ = 0;

	ku::Camera camera_;
	ku::Point_light point_light_;

	QTimer timer_;

	uint32_t frame_ = 0;

	QOpenGLFunctions* gl = nullptr;

	QOpenGLContext* context_;

	QOpenGLVertexArrayObject scene_vao_;
	QOpenGLBuffer vert_buff_{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer uv_buff_{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer normal_buff_{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer tangent_buff_{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer bitangent_buff_{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer index_buff_{ QOpenGLBuffer::IndexBuffer };

	QOpenGLShaderProgram program_;
	QOpenGLShaderProgram normalbuffer_program_;

	QOpenGLTexture* texture1_ = nullptr;
	QOpenGLTexture* texture2_ = nullptr;

	ku::Scene scene_;

	std::map<std::string, GLuint> attrib_locs_;
	std::map<std::string, GLuint> uniform_locs_;

	void initializeGL() override;

	void resizeGL(const int w, const int h) override;

	void paintGL() override;

	void swap_model(const char* uri);

	void handle_user_control();

};

