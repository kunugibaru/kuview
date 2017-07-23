#include "stdafx.h"
#include "kuview.h"

kuview::kuview(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	sceneinfo_model_ = new Kv_sceneinfo_model();
	ui.tv_modelinfo->setModel(sceneinfo_model_);

	connect(
		ui.le_modeluri, &QLineEdit::textChanged, 
		[&](const QString& uri) {
		
		QFileInfo f(uri);
		if (f.exists()) {
			ui.glw_main->model_changed_ = true;
			ui.glw_main->model_uri_ = uri;

			const auto& scene = ku::read_scene(uri.toStdString().c_str());

			sceneinfo_model_->change_model(scene);

			
		}
	});
	
	connect(ui.sb_intensity, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[&](const int intensity) {
		ui.glw_main->point_light_.intensity_ = intensity;
	});

	connect(ui.sb_ambient, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[&](const int intensity) {
		ui.glw_main->point_light_.ambient_ = intensity * 0.1f;
	});

	connect(
		ui.te_fs, &QPlainTextEdit::textChanged, 
		[this]() {
		
		const QString& log = 
			ui.glw_main->change_shader(ui.te_fs->toPlainText().toStdString().c_str());
		
		this->ui.te_log->setPlainText(log);
	});

	connect(
		ui.le_basecolor, &QLineEdit::textChanged,
		[this](const QString& uri) {
		
		QFileInfo f(uri);
		if (f.exists()) {
			ui.glw_main->change_texture(0, uri);
		}
	});
	connect(
		ui.le_normal, &QLineEdit::textChanged,
		[this](const QString& uri) {

		QFileInfo f(uri);
		if (f.exists()) {
			ui.glw_main->change_texture(1, uri);
		}
	});
	connect(
		ui.le_roughness, &QLineEdit::textChanged,
		[this](const QString& uri) {

		QFileInfo f(uri);
		if (f.exists()) {
			ui.glw_main->change_texture(2, uri);
		}
	});
	connect(
		ui.le_metallic, &QLineEdit::textChanged,
		[this](const QString& uri) {

		QFileInfo f(uri);
		if (f.exists()) {
			ui.glw_main->change_texture(3, uri);
		}
	});

	this->ui.te_fs->setPlainText(ku::normal_fs);
}

void kuview::showEvent(QShowEvent * e)
{
	ku::Scene scene
		= ui.glw_main->swap_model(":/kuview/Resources/sphere.obj");
	sceneinfo_model_->change_model(scene);

	QMainWindow::showEvent(e);
}

