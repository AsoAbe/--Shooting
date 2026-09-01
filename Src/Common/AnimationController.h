#pragma once
#include <string>
#include <map>
#include "../Object/CharacterBase.h"
class SceneManager;
class CharacterModelData;

using ANIM = CharacterBase::ANIM;

/// <summary>
/// CharacterModelData対応版
/// </summary>
class AnimationController
{
	
public :
	// 無効なモデルID
	static constexpr int INVALID_MODEL_ID = -1;

	// 無効なアニメーション番号
	static constexpr int INVALID_ANIMATION_NO = -1;

	// 無効なアニメーション種別
	static constexpr int INVALID_PLAY_TYPE = -1;

	// デフォルトのアニメーション番号
	static constexpr int DEFAULT_ANIM_INDEX = 0;

	// デフォルトのアニメーション速度
	static constexpr float DEFAULT_SPEED = 0.0f;

	// デフォルトのアニメーションステップ
	static constexpr float DEFAULT_STEP = 0.0f;

	// 終了ステップを自動取得する場合の値
	static constexpr float USE_ANIM_TOTAL_TIME = -1.0f;

	// アニメーションの通常再生方向
	static constexpr float PLAY_FORWARD = 1.0f;

	// アニメーションの逆再生方向
	static constexpr float PLAY_REVERSE = -1.0f;

	// 1秒を表す値
	static constexpr float SECONDS_PER_FRAME = 1.0f;

	struct Animation
	{
		int model = INVALID_MODEL_ID;
		int attachNo = INVALID_ANIMATION_NO;
		int animIndex = DEFAULT_ANIM_INDEX;
		float speed = DEFAULT_SPEED;
		float totalTime = DEFAULT_STEP;
		float step = DEFAULT_STEP;
	};

	// コンストラクタ
	AnimationController(int modelId,const CharacterModelData& modelData);
	// デストラクタ
	~AnimationController(void);

	// アニメーション追加
	void Add(ANIM type, int handle, float speed);

	// アニメーション再生
	void Play(ANIM type, bool isLoop = true,
		float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false);

	void Update(void);

	// アニメーション終了後に繰り返すループステップ
	void SetEndLoop(float startStep, float endStep, float speed);

	// 再生中のアニメーション
	int GetPlayType(void) const;

	// 再生終了
	bool IsEnd(void) const;

private :

	// モデルのハンドルID
	int modelId_;

	// キャラモデルのデータ
	const CharacterModelData& modelData_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	int playType_;
	Animation playAnim_;

	// アニメーションをループするかしないか
	bool isLoop_;

	// アニメーションを止めたままにする
	bool isStop_;

	// アニメーション終了後に繰り返すループステップ
	float stepEndLoopStart_;
	float stepEndLoopEnd_;
	float endLoopSpeed_;

	// 逆再生
	float switchLoopReverse_;

};

