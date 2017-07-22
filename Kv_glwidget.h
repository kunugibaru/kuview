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
	Qt::Key last_key_ = Qt::Key::Key_unknown; 
	bool is_left_mousebutton_pressed_ = false;

	QPoint last_mouse_pos_;

	// OpenGL objects
	QOpenGLFunctions* gl = nullptr;
	QOpenGLContext* context_;

	struct Vertex_buffer
	{
		QOpenGLVertexArrayObject* vao_ = new QOpenGLVertexArrayObject;
		QOpenGLBuffer* vert_buff_ = new QOpenGLBuffer{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer* uv_buff_ = new QOpenGLBuffer{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer* normal_buff_ = new QOpenGLBuffer{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer* tangent_buff_ = new QOpenGLBuffer{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer* bitangent_buff_ = new QOpenGLBuffer{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer* index_buff_ = new QOpenGLBuffer{ QOpenGLBuffer::IndexBuffer };

		
	};

	std::vector<std::unique_ptr<Vertex_buffer>> vert_buffers_;

	QOpenGLShaderProgram program_;
	QOpenGLShaderProgram normalbuffer_program_;

	QOpenGLTexture* texture1_ = nullptr;
	QOpenGLTexture* texture2_ = nullptr;

	// Scene entities
	ku::Scene scene_;
	ku::Camera camera_;
	ku::Point_light point_light_;

	// Uniform
	uint32_t frame_ = 0;

	QTimer timer_;


	void initializeGL() override;

	void resizeGL(const int w, const int h) override;

	void paintGL() override;

	/**
	呼び出しもとの Widget がシーン情報を参照できるよう戻り値に。
	*/
	ku::Scene swap_model(const char* uri);

	void handle_user_control();

	void keyPressEvent(QKeyEvent* e) override;

	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	
};

