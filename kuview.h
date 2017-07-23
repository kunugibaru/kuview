#pragma once
#include "stdafx.h"
#include <QtWidgets/QMainWindow>
#include "ui_kuview.h"
#include "Kv_sceneinfo_model.h"
#include "common.h"

class kuview : public QMainWindow
{
	Q_OBJECT

public:
	Kv_sceneinfo_model* sceneinfo_model_ = nullptr;

	kuview(QWidget *parent = Q_NULLPTR);

	void showEvent(QShowEvent* e) override;
private:
	Ui::kuviewClass ui;
};
