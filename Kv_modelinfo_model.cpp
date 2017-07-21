#include "stdafx.h"
#include "Kv_modelinfo_model.h"


namespace ku
{
	namespace placeholders
	{
		static ku::Scene emplty_scene;
	}
}

Kv_modelinfo_model::Kv_modelinfo_model()
	: QAbstractTableModel()
{
	scene_ = &ku::placeholders::emplty_scene;
}


Kv_modelinfo_model::~Kv_modelinfo_model()
{
}
