#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include"SceneBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//コンストラクタ
SceneBase::SceneBase(void)
{
	
}
//デストラクタ
SceneBase::~SceneBase(void)
{

}
//初期化
bool SceneBase::Init(void)
{
	return true;
}
void SceneBase::Update(void)
{

}
//描画
void SceneBase::Draw(void)
{

}
//解放
bool SceneBase::Release(void)
{
	return true;
}