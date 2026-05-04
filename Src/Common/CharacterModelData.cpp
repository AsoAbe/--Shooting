#include"../Application.h"
#include"CharacterModelData.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CharacterModelData::CharacterModelData()
{
	Clear();
	modelScale_ = 1;
	//配列を作る
	for (int i = 0; i < static_cast<int>(ANIM::MAX); i++)
	{
		anim_.emplace_back();
	}
	fallEndLoopSpd_ = DEFAULT_ENDLOOPSPD_FALL;
	
}

CharacterModelData::~CharacterModelData()
{
	ReleaseModel();
}

void CharacterModelData::Clear()
{
	anim_.clear();
	modelFileName_ = "";
	model_ = -1;
	gliderS_ = 0;
	gliderE_ = 0;
	onGroundS_ = 0;
}

void CharacterModelData::LoadModel()
{
	if (modelFileName_ != "" && model_ == -1)
	{
		model_ = MV1LoadModel((Application::PATH_MODEL+modelFileName_).c_str());
	}
}

CharacterModelData::AnimData::~AnimData()
{
	ReleaseAModel();
}

int CharacterModelData::AnimData::LoadAModel()
{
	if (handle != -1)
	{
		return handle;
	}
	handle= MV1LoadModel(fileName.c_str());
	return handle;
}

int CharacterModelData::AnimData::ReleaseAModel()
{
	if (fileName == "" || handle == -1)
	{
		// 次に読み込むモデルの物理演算モードをリアルタイム物理演算にする
		MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
	}
	int ret = MV1DeleteModel(handle);
	handle = -1;
	return ret;
}

void CharacterModelData::ReleaseModel()
{
	if (model_ != -1)
	{
		MV1DeleteModel(model_);
		model_ = -1;
	}
	Clear();
}

float CharacterModelData::GetModelScale() const
{
	return modelScale_;
}

int CharacterModelData::GetAnimIndex(ANIM anim)const
{
	int i =CharacterBase::GetAnimType(anim);
	if (anim_.size() <= i)
	{
		//範囲外
		return -1;
	}
	return anim_[i].index;
}

//const std::string& CharacterModelData::GetAnimPath(ANIM anim)const
//{
//	int i = CharacterBase::GetAnimType(anim);
//	if (anim_.size() <= i)
//	{
//		//範囲外
//		return anim_[0].fileName;
//	}
//	return anim_[i].fileName;
//}

int CharacterModelData::GetAnimMHandle(ANIM anim)const
{
	int i = CharacterBase::GetAnimType(anim);
	if (anim_.size() <= i)
	{
		//範囲外
		return -1;
	}
	return anim_[i].handle;
}

int CharacterModelData::GetAnimSpeed(ANIM anim) const
{
	int i = CharacterBase::GetAnimType(anim);
	if (anim_.size() <= i)
	{
		//範囲外
		return 0;
	}
	return anim_[i].speed;
}

void CharacterModelData::SetAnimPath(ANIM anim, const std::string& path,int index,int speed)
{
	int i = CharacterBase::GetAnimType(anim);
	if (anim_.size() <= i)
	{
		//範囲外
		return;
	}
	anim_[i].fileName = Application::PATH_MODEL+path;
	anim_[i].index = index;
	anim_[i].speed = speed;
	//モデルロード
	anim_[i].LoadAModel();
}

void CharacterModelData::SetJumpOption(int gliderS, int gliderE, int onGroundS)
{
	gliderS_ = gliderS;
	gliderE_ = gliderE;
	onGroundS_ = onGroundS;
}

int CharacterModelData::GetGliderS() const
{
	return gliderS_;
}

int CharacterModelData::GetGliderE() const
{
	return gliderE_;
}

int CharacterModelData::GetOnGroundS() const
{
	return onGroundS_;
}

int CharacterModelData::GetFallEndLoopSpd() const
{
	return fallEndLoopSpd_;
}
