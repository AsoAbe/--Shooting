#pragma once
#include<string>
#include<DxLib.h>

class ObjectModelData
{
public:

	// 無効なモデルハンドル
	static constexpr int INVALID_MODEL_HANDLE = -1;

	// デフォルトのモデルサイズ
	static constexpr float DEFAULT_MODEL_SCALE = 1.0f;

	// デフォルトの回転値
	static constexpr float DEFAULT_ROTATION = 0.0f;

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

