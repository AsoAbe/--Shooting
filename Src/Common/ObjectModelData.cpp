#include <DxLib.h>
#include"../Application.h"
#include "ObjectModelData.h"

ObjectModelData::ObjectModelData()
{
	Clear();
}

ObjectModelData::~ObjectModelData()
{
	ReleaseModel();
}

void ObjectModelData::Clear()
{
	modelFileName_ = "";
	model_ = -1;
	modelScale_ = 1;
	modelRot_ = VGet(0, 0, 0);
}

void ObjectModelData::LoadModel()
{
	if (modelFileName_ != "" && model_ == -1)
	{
		model_ = MV1LoadModel((Application::PATH_MODEL + modelFileName_).c_str());
	}
}

void ObjectModelData::ReleaseModel()
{
	if (model_ != -1)
	{
		MV1DeleteModel(model_);
	}
	Clear();
}

float ObjectModelData::GetModelScale() const
{
	return modelScale_;
}
