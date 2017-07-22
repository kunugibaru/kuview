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
			const ku::Scene& scene 
				= ui.glw_main->swap_model(uri.toStdString().c_str());

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

	
}

void kuview::showEvent(QShowEvent * e)
{
	const ku::Scene& scene
		= ui.glw_main->swap_model("D:/temps/machine_01.obj");
	sceneinfo_model_->change_model(scene);

	QMainWindow::showEvent(e);
}

