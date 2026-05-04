#pragma once
#include<string>
#include<DxLib.h>

class ObjectModelData
{
public:
	//モデルのファイル名
	std::string modelFileName_;
	//キャラのモデルのハンドル
	int model_;
	//モデルのサイズ調整
	float modelScale_;

	//モデルのローカルローテーション
	VECTOR modelRot_;

	ObjectModelData();
	~ObjectModelData();

	//すべてを-1にする。主に解放処理の後に実行
	void Clear();

	/// <summary>
	/// 事前に設定されたファイル名からmv1をロードする
	/// </summary>
	void LoadModel();

	/// <summary>
	/// ロード済みのハンドルを解放する
	/// </summary>
	void ReleaseModel();

	float GetModelScale()const;
private:
};

