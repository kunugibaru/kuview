#include "stdafx.h"
#include "Kv_glwidget.h"

namespace ku
{

}



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
	
	{
		const auto& form = context_->format();

		qInfo() << form.version();
	}
	

	const auto& make_shader = [](
		QOpenGLShaderProgram& program,
		const char* vs,
		const char* fs) {
	
		bool result = true;
		result = program.addShaderFromSourceCode(QOpenGLShader::Vertex, vs);

		if (!result) {
			throw ku::ShaderCompilationError(program.log().toStdString().c_str());
		}

		result = program.addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
		if (!result) {
			throw ku::ShaderCompilationError(program.log().toStdString().c_str());
		}
		program.link();
	};
	
	make_shader(program_, ku::basic_vs, ku::basic_fs);
	make_shader(normalbuffer_program_, ku::basic_vs, ku::normal_fs);

	//normalbuffer_program_.addShaderFromSourceCode(QOpenGLShader::Fragment, basic_fs);
	//normalbuffer_program_.link();

	const auto& create_texture = [this](const char* texpath) -> QOpenGLTexture* {
		
		auto* texture = new QOpenGLTexture(QImage(texpath).mirrored());
		texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
		texture->bind();
		texture->setMagnificationFilter(QOpenGLTexture::Linear);
		texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		texture->setWrapMode(QOpenGLTexture::Repeat);
		return texture;
	};

	texture1_ = create_texture(":/kuview/Resources/Default_basecolor.tga");
	texture2_ = create_texture(":/kuview/Resources/Default_normal.tga");
	texture3_ = create_texture(":/kuview/Resources/Default_roughness.tga");
	texture4_ = create_texture(":/kuview/Resources/Default_metallic.tga");
	
	qInfo() << "GL widget initialized";
}

void Kv_glwidget::resizeGL(const int w, const int h)
{
	width_ = w;
	height_ = h;

}

void Kv_glwidget::paintGL()
{
	if (fs_changed_) {
		normalbuffer_program_.removeAllShaders();
		normalbuffer_program_.addShaderFromSourceCode(QOpenGLShader::Vertex, ku::basic_vs);
		normalbuffer_program_.addShader(user_fs_);
		normalbuffer_program_.link();
		fs_changed_ = false;

	}

	if (model_changed_) {
		swap_model(model_uri_.toStdString().c_str());
		model_changed_ = false;
	}

	if (changed_tex_ != -1) {
		gl->glActiveTexture(GL_TEXTURE0 + changed_tex_);
		
		const auto& setup_texture = [](QOpenGLTexture* tex, const QOpenGLTexture::TextureFormat& tf) {
			tex->bind();
			tex->setFormat(tf);
			tex->setMagnificationFilter(QOpenGLTexture::Linear);
			tex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
			tex->setWrapMode(QOpenGLTexture::Repeat);
		};
		
		if (changed_tex_ == 0) {
			texture1_ = new QOpenGLTexture(QImage(tex_uri_).mirrored());
			setup_texture(texture1_, QOpenGLTexture::RGBA8_UNorm);
		}
		else if (changed_tex_ == 1) {
			texture2_ = new QOpenGLTexture(QImage(tex_uri_).mirrored());
			setup_texture(texture2_, QOpenGLTexture::RGBA8_UNorm);
		}
		else if (changed_tex_ == 2) {
			texture3_ = new QOpenGLTexture(QImage(tex_uri_).mirrored());
			setup_texture(texture3_, QOpenGLTexture::R8_UNorm);
		}
		else if (changed_tex_ == 3) {
			texture4_ = new QOpenGLTexture(QImage(tex_uri_).mirrored());
			setup_texture(texture4_, QOpenGLTexture::R8_UNorm);
		}
		
		changed_tex_ = -1;
	}

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
		gl->glActiveTexture(GL_TEXTURE2);
		texture3_->bind();
		gl->glUniform1i(program.uniformLocation("Sampler_Roughness"), 2);
		gl->glActiveTexture(GL_TEXTURE3);
		texture4_->bind();
		gl->glUniform1i(program.uniformLocation("Sampler_Metallic"), 3);
	};

	for (const auto& vattr : this->vert_buffers_) {
		// Clear All
		glViewport(0, 0, width_, height_);
		gl->glClearColor(0.2, 0.2, 0.2, 1);


		// ¶
		glViewport(0, 0, width_ * 0.5, height_);
		//vattr->vao_.bind();

		set_uniforms(program_);

		gl->glDrawElements(
			GL_TRIANGLES, vattr->size_, GL_UNSIGNED_INT, (void*)0);
		
		// ‰E
		glViewport(width_ * 0.5, 0, width_ * 0.5, height_);
		set_uniforms(normalbuffer_program_);

		gl->glDrawElements(
			GL_TRIANGLES, vattr->size_, GL_UNSIGNED_INT, (void*)0);

		//vattr->vao_.release();
	}


	texture1_->release();
	texture2_->release();
	texture3_->release();
	texture4_->release();
}

ku::Scene Kv_glwidget::swap_model(const char * uri)
{

	scene_ = ku::read_scene(uri);

	this->vert_buffers_.clear();

	/**
	@param(data) data ‚Í std::vector<float> ‚© std::vector<uint32_t>
	*/
	const auto& init_and_set = [this](
		QOpenGLBuffer& buffer,
		const GLuint index,
		const auto& data,
		const GLenum typ,
		const uint32_t tuplesize
		)
	{
		buffer.create();
		buffer.bind();
		gl->glBufferData(buffer.type(), sizeof(float) * data.size(), 0, GL_STATIC_DRAW);
		gl->glBufferSubData(buffer.type(), 0, sizeof(float) * data.size(), data.data());
		
		gl->glEnableVertexAttribArray(index);
		gl->glVertexAttribPointer(index, tuplesize, typ, GL_TRUE, 0, (void*)0);
	};

	for (size_t i = 0; i < scene_.meshes_.size(); i++) {
		const auto& mesh = scene_.meshes_.at(i);
		auto& vattr = std::make_unique<Vertex_buffer>();
		
		vattr->vao_.bind();

		init_and_set(vattr->vert_buff_, 0, mesh.verts_, GL_FLOAT, 3);
		init_and_set(vattr->uv_buff_, 1, mesh.texcoords_, GL_FLOAT, 2);
		init_and_set(vattr->normal_buff_, 2, mesh.normals_, GL_FLOAT, 3);
		init_and_set(vattr->tangent_buff_, 3, mesh.tangents_, GL_FLOAT, 3);
		init_and_set(vattr->bitangent_buff_, 4, mesh.bitangents_, GL_FLOAT, 3);
		init_and_set(vattr->index_buff_, 5, mesh.faces_, GL_UNSIGNED_INT, 3);

		vattr->size_ = mesh.faces_.size();
		vattr->vao_.release();

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

QString Kv_glwidget::change_shader(const QString& source)
{
	QOpenGLShader* shader = new QOpenGLShader{ QOpenGLShader::Fragment };
	bool result = shader->compileSourceCode(source);
	if (result == true) {
		user_fs_ = shader;
		fs_changed_ = true;
		return "ok";
	}
	else {
		return shader->log();
	}

}

void Kv_glwidget::change_texture(const uint8_t index, const QString& path)
{
	tex_uri_ = path;
	changed_tex_ = index;
}
