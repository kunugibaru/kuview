#pragma once
#include "stdafx.h"
#include <QtWidgets/QMainWindow>
#include "ui_kuview.h"
#include "Kv_modelinfo_model.h"

class kuview : public QMainWindow
{
	Q_OBJECT

public:
	Kv_modelinfo_model* modelinfo_model_ = nullptr;

	kuview(QWidget *parent = Q_NULLPTR);

private:
	Ui::kuviewClass ui;
};
