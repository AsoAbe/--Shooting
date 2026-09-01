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
	model_ = INVALID_MODEL_HANDLE;
	modelScale_ = DEFAULT_MODEL_SCALE;
	modelRot_ = VGet(DEFAULT_ROTATION, DEFAULT_ROTATION, DEFAULT_ROTATION);
}

void ObjectModelData::LoadModel()
{
	if (modelFileName_ != "" && model_ == INVALID_MODEL_HANDLE)
	{
		model_ = MV1LoadModel((Application::PATH_MODEL + modelFileName_).c_str());
	}
}

void ObjectModelData::ReleaseModel()
{
	if (model_ != INVALID_MODEL_HANDLE)
	{
		MV1DeleteModel(model_);
	}
	Clear();
}

float ObjectModelData::GetModelScale() const
{
	return modelScale_;
}
