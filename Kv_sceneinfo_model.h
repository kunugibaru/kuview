#pragma once
#include <stdafx.h>
#include "common.h"

class Kv_sceneinfo_model : public QAbstractTableModel
{
	/**
	各データが QVariant なのは、data() の戻り値となる際に
	レキシカルキャストが起きないようにするためです。
	*/
	struct Record
	{
		QVariant material_name_ = "";
		QVariant vertex_num_ = "";
		QVariant face_num_ = "";
	};

	std::vector<Record> records_;

public:
	Kv_sceneinfo_model();
	~Kv_sceneinfo_model();

	/**
	コンストラクタでプレースホルダーが呼ばれるので
	NULLチェック不要
	*/ 
	ku::Scene* scene_ = nullptr;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant headerData(
		int section, 
		Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	void change_model(const ku::Scene& scene);
};

