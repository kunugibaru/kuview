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
}


class Kv_glwidget :
	public QOpenGLWidget
{
public:
	Kv_glwidget(QWidget* parent);
	~Kv_glwidget();

	// ユーザーコントロール関連
	uint32_t width_ = 0;
	uint32_t height_ = 0;
	Qt::Key last_key_ = Qt::Key::Key_unknown; 
	bool is_left_mousebutton_pressed_ = false;

	QPoint last_mouse_pos_;

	// OpenGL オブジェクト
	QOpenGLFunctions* gl = nullptr;
	QOpenGLContext* context_;

	struct Vertex_buffer
	{
		QOpenGLVertexArrayObject vao_{};
		QOpenGLBuffer vert_buff_{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer uv_buff_{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer normal_buff_{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer tangent_buff_{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer bitangent_buff_{ QOpenGLBuffer::VertexBuffer };
		QOpenGLBuffer index_buff_{ QOpenGLBuffer::IndexBuffer };

		uint32_t size_ = 0;
	};

	std::vector<std::unique_ptr<Vertex_buffer>> vert_buffers_;

	QOpenGLShaderProgram program_;
	QOpenGLShaderProgram normalbuffer_program_;

	QOpenGLTexture* texture1_ = nullptr;
	QOpenGLTexture* texture2_ = nullptr;
	QOpenGLTexture* texture3_ = nullptr;
	QOpenGLTexture* texture4_ = nullptr;

	/**
	ユーザーが書き換えたシェーダーをコンパイルして一時的にメンバーに格納する。
	メインスレッドでリンクする。
	*/
	std::atomic<bool> fs_changed_ = false;
	QOpenGLShader* user_fs_;

	std::atomic<bool> model_changed_ = false;
	QString model_uri_;

	std::atomic<int8_t> changed_tex_ = -1;
	QString tex_uri_;

	// エンティティ
	ku::Scene scene_;
	ku::Camera camera_;

	struct Point_light
	{
		QVector3D position_{ 1,5,3 };
		QVector3D color_{ 1,1,1 };
		float intensity_ = 10;
		float ambient_ = 0.1;
	};

	Point_light point_light_;

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
	
	/**
	以下の関数は別スレッドから実行される。
	(直接 OpenGL コンテキストを操作してはいけない。)
	*/

	QString change_shader(const QString& source);
	
	void change_texture(const uint8_t index, const QString& path);

};

