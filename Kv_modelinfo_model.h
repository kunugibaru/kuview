#pragma once
#include <stdafx.h>
#include "common.h"

class Kv_modelinfo_model : public QAbstractTableModel
{
public:
	Kv_modelinfo_model();
	~Kv_modelinfo_model();

	/**
	コンストラクタでプレースホルダーが呼ばれるので
	NULLチェック不要
	*/ 
	ku::Scene* scene_ = nullptr;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override
	{
		return scene_->meshes_.size();
	}

	int columnCount(const QModelIndex &parent = QModelIndex()) const override
	{
		return 3;
	}

	QVariant data(const QModelIndex& index, int role) const override
	{
		return "weee";
	}

	void change_model(const QString& stri)
	{
		
	}
};

