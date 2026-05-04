
#include "../Common/AsoUtility.h"
#include "../Application.h"
#include "../Common/ObjectModelData.h"
#include "ObjectBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

ObjectBase::ObjectBase(const ObjectModelData& modelData) :baseModel_(modelData)
{
    pos_ = AsoUtility::VECTOR_ZERO;
    rot_ = AsoUtility::VECTOR_ZERO;
    scale_ = { 1,1,1 };
    vec_ = AsoUtility::VECTOR_ZERO;
    localPos_ = AsoUtility::VECTOR_ZERO;
    modelId_ = -1;

    quaRot_ = Quaternion::Identity();
    quaLocalRot_ = Quaternion::Euler(baseModel_.modelRot_);
    isActive_ = false;
   
}
ObjectBase::~ObjectBase()
{

}

void ObjectBase::Init(void)
{
    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);

    SetParam();
   
    // モデルの読込(複製)
    modelId_ = MV1DuplicateModel(baseModel_.model_);
    //マテリアルの環境光を設定
    MV1SetMaterialAmbColor(modelId_, 0, GetColorF(0.5f, 0.5f, 0.5f, 1));

    //マトリクスでモデルを制御
    SetMatrixModel();

    ////回転を設定
    //SetRotation();

    //有効化する
    isActive_ = true;
}

void ObjectBase::Update(void)
{
    if (!isActive_)
    {
        return;
    }
    //rotを更新
    rot_ = quaRot_.ToEuler();
    //マトリクスでモデルを制御
    SetMatrixModel();
}

void ObjectBase::Draw(void)
{
    if (!isActive_)
    {
        //アクティブではない場合は描画しない
        return;
    }
    // モデルの描画
    MV1DrawModel(modelId_);
}

void ObjectBase::Release(void)
{
    // モデルのメモリ解放
    MV1DeleteModel(modelId_);
    modelId_ = -1;
    // 物理演算の状態をリセット
    MV1PhysicsResetState(modelId_);
}

bool ObjectBase::IsActive()const
{
    return isActive_;
}

void ObjectBase::SetActive(bool b)
{
    isActive_ = b;
}

VECTOR ObjectBase::GetPos()const
{
    return pos_;
}

void ObjectBase::SetPos(VECTOR pos)
{
    pos_ = pos;
}

void ObjectBase::SetMatrixModel(void)
{
    // 大きさ
    MATRIX matScale = MGetScale(VScale(scale_,baseModel_.GetModelScale()));
    // 回転
    Quaternion mixRot = Quaternion::Identity();
    // ローカル回転を加える(順番に注意)
    mixRot = Quaternion::Mult(quaRot_, quaLocalRot_);
    // 位置
    MATRIX matPos = MGetTranslate(pos_);
    // 行列の合成
    MATRIX mat = MGetIdent();

    //ローカル座標
    mat = MMult(mat, MGetTranslate(localPos_));
    // 大きさ
    mat = MMult(mat, matScale);
    // 回転
    mat = MMult(mat, mixRot.ToMatrix());
    // 位置
    mat = MMult(mat, matPos);
    //---------------------------------
    // 行列(大きさ、角度、位置)をモデルに設定
    MV1SetMatrix(modelId_, mat);
    // オイラー角を求める
    rot_ = quaRot_.ToEuler();
}

