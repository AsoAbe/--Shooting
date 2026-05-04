#pragma once
#include<DxLib.h>
#include<string>
#include<vector>
#include"../Object/CharacterBase.h"

using ANIM = CharacterBase::ANIM;

/// <summary>
/// キャラクターのモデルのハンドルとアニメーションのハンドルを格納する
/// </summary>
class CharacterModelData
{
public:
	//基準のアニメーション速度
	static constexpr int DEFAULT_ANIMSPD = 30;
	static constexpr int DEFAULT_ENDLOOPSPD_FALL = 2;

	//モデルのファイル名
	std::string modelFileName_;
	//アニメーションのソースのモデルのファイル名。アニメーションがモデルと同じファイル内の場合は何も入れない
	std::string idleFileName_;
	std::string runFileName_;
	//キャラのモデルのハンドル
	int model_;
	//モデルのサイズ調整
	float modelScale_;

	CharacterModelData();
	~CharacterModelData();

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

	int GetAnimIndex(ANIM anim)const;
	//const std::string& GetAnimPath(ANIM anim)const;
	int GetAnimSpeed(ANIM anim)const;
	int GetAnimMHandle(ANIM anim)const;
	/// <summary>
	/// アニメーションのパスとインデックスを編集する。
	/// </summary>
	void SetAnimPath(ANIM anim, const std::string& path,int index,int speed = CharacterModelData::DEFAULT_ANIMSPD);
	/// <summary>
	/// ジャンプアニメーション関係の値を編集する
	/// </summary>
	/// <param name="gliderS">滑空開始</param>
	/// <param name="gliderE">滑空の終わり(ループ)</param>
	/// <param name="onGroundS">着地アニメーションの開始地点</param>
	void SetJumpOption(int gliderS, int gliderE,int onGroundS);

	/// <summary>
	/// 滑空開始
	/// </summary>
	/// <returns></returns>
	int GetGliderS()const;
	/// <summary>
	/// 滑空の終わり
	/// </summary>
	/// <returns></returns>
	int GetGliderE()const;
	/// <summary>
	/// 着地アニメーションの開始地点
	/// </summary>
	/// <returns></returns>
	int GetOnGroundS()const;
	/// <summary>
	/// 落下アニメーションのEndLoopのスピード
	/// </summary>
	/// <returns></returns>
	int GetFallEndLoopSpd()const;
private:
	struct AnimData
	{
	public:
		//ファイル名
		std::string fileName;
		//ロードしたハンドル
		int handle;
		//アニメーションのインデックス
		int index;
		//アニメーション速度
		int speed;
		//コンストラクタ
		AnimData():fileName(""),handle(-1), index(-1), speed(DEFAULT_ANIMSPD) {}
		~AnimData();
		/// <summary>
		/// MV1LoadModelを実行
		/// </summary>
		/// <returns>失敗時に-1を返す</returns>
		int LoadAModel();
		/// <summary>
		/// MV1DeleteModelを実行
		/// </summary>
		/// <returns></returns>
		int ReleaseAModel();
	};
	////アニメーションのソースのモデルのハンドル。モデルと同じファイル内の場合は-1
	//std::vector<int> animSrc_;
	//アニメーションのソースのモデルのファイル名。アニメーションがモデルと同じファイル内の場合は何も入れない
	//std::vector<std::string> animFileName_;
	////インデックス
	//std::vector<int> animIndex_;
	std::vector<AnimData> anim_;

	int gliderS_;
	int gliderE_;
	int onGroundS_;
	int fallEndLoopSpd_;
};
