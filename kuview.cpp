#include "stdafx.h"
#include "kuview.h"

kuview::kuview(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	modelinfo_model_ = new Kv_modelinfo_model();
	ui.tv_modelinfo->setModel(modelinfo_model_);

	connect(
		ui.le_modeluri, &QLineEdit::textChanged, 
		[&](const QString& uri) {
		
		QFileInfo f(uri);
		if (f.exists()) {
			ui.glw_main->swap_model(uri.toStdString().c_str());
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
