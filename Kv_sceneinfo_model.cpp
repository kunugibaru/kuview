#include "stdafx.h"
#include "Kv_sceneinfo_model.h"


namespace ku
{
	namespace placeholders
	{
		static ku::Scene emplty_scene;
	}
}

Kv_sceneinfo_model::Kv_sceneinfo_model()
	: QAbstractTableModel()
{
	scene_ = &ku::placeholders::emplty_scene;
}


Kv_sceneinfo_model::~Kv_sceneinfo_model()
{

}

int Kv_sceneinfo_model::rowCount(const QModelIndex & parent) const
{
	return records_.size();
}

int Kv_sceneinfo_model::columnCount(const QModelIndex & parent) const
{
	return 3;
}

QVariant Kv_sceneinfo_model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
		return QAbstractTableModel::headerData(section, orientation, role);
	}

	switch (section)
	{
	case 0:
		return "Material Name";
	case 1:
		return "Vertex Count";
	case 2:
		return "Face Count";
	default:
		break;
	}

	return "";
}

QVariant Kv_sceneinfo_model::data(const QModelIndex &index, int role) const
{
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	auto& rec = records_.at(index.row());

	switch (index.column())
	{
	case 0:
		return rec.material_name_;
	case 1:
		return rec.vertex_num_;
	case 2:
		return rec.face_num_;
	default:
		break;
	}
	return "";
}

void Kv_sceneinfo_model::change_model(const ku::Scene& scene)
{
	this->beginResetModel();
	
	records_.clear();

	for (size_t i = 0; i < scene.meshes_.size(); i++) {
		Record record;
		
		const ku::Mesh& mesh = scene.meshes_.at(i);

		if (scene.materials_.size() > 0) {
			record.material_name_ = scene.materials_.at(mesh.material_index_).c_str();
		}
		else {
			record.material_name_ = "None";
		}
		record.vertex_num_ = std::to_string(mesh.verts_.size()).c_str();
		record.face_num_ = std::to_string(mesh.faces_.size()).c_str();

		records_.push_back(std::move(record));
	}

	this->endResetModel();
}
