#include <DxLib.h>
#include "../Application.h"
#include "./CharacterModelData.h"
#include "../Object/CharacterBase.h"
#include "AnimationController.h"

using ANIM = CharacterBase::ANIM;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

AnimationController::AnimationController(int modelId,const CharacterModelData& modelData):modelData_(modelData)
{
	modelId_ =modelId;

	playType_ = INVALID_PLAY_TYPE;
	isLoop_ = false;
	isStop_ = false;

	switchLoopReverse_ = PLAY_FORWARD;
	endLoopSpeed_ = DEFAULT_SPEED;
	stepEndLoopStart_ = DEFAULT_STEP;
	stepEndLoopEnd_ = DEFAULT_STEP;
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(ANIM type, int handle, float speed)
{
	int typeI = CharacterBase::GetAnimType(type);

	Animation anim;

	anim.model = handle;
	anim.animIndex = typeI;
	anim.speed = speed;

	if (animations_.count(typeI) == 0)
	{
		// 入れ替え
		animations_.emplace(typeI, anim);
	}
	else
	{
		// 追加
		animations_[typeI].model = anim.model;
		animations_[typeI].animIndex = anim.animIndex;
		animations_[typeI].attachNo = anim.attachNo;
		animations_[typeI].totalTime = anim.totalTime;
	}
}

void AnimationController::Play(ANIM type, bool isLoop,
	float startStep, float endStep, bool isStop, bool isForce)
{
	int typeI = CharacterBase::GetAnimType(type);
	if (playType_ != typeI || isForce) {

		if (playType_ != -1)
		{
			// モデルからアニメーションを外す
			playAnim_.attachNo = MV1DetachAnim(modelId_, playAnim_.attachNo);
		}

		// アニメーション種別を変更
		playType_ = typeI;
		playAnim_ = animations_[typeI];

		// 初期化
		playAnim_.step = startStep;

		// モデルにアニメーションを付ける
		int animIdx = modelData_.GetAnimIndex(type);
		playAnim_.attachNo = MV1AttachAnim(modelId_, animIdx, playAnim_.model);

		// アニメーション総時間の取得
		if (endStep < DEFAULT_STEP)
		{
			endStep = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);
		}
		playAnim_.totalTime = endStep;

		// アニメーションループ
		isLoop_ = isLoop;

		// アニメーションしない
		isStop_ = isStop;

		switchLoopReverse_ = PLAY_FORWARD;
		float endLoopSpd = CharacterModelData::DEFAULT_ANIMSPD;
		if (type == ANIM::FALLING)
		{
			endLoopSpd = modelData_.GetFallEndLoopSpd();
		}
		SetEndLoop(startStep, endStep, endLoopSpd);
	}

}

void AnimationController::Update(void)
{

	// 経過時間の取得
	float deltaTime = SECONDS_PER_FRAME / Application::FPS;

	if (!isStop_)
	{
		// 再生
		playAnim_.step += (deltaTime * playAnim_.speed * switchLoopReverse_);

		// アニメーション終了判定
		bool isEnd = false;
		if (switchLoopReverse_ > DEFAULT_STEP)
		{
			// 通常再生の場合
			if (playAnim_.step > playAnim_.totalTime)
			{
				isEnd = true;
			}
		}
		else
		{
			// 逆再生の場合
			if (playAnim_.step < playAnim_.totalTime)
			{
				isEnd = true;
			}
		}

		if (isEnd)
		{
			// アニメーションが終了したら
			if (isLoop_)
			{
				// ループ再生
				if (stepEndLoopStart_ > DEFAULT_STEP)
				{
					// アニメーション終了後の指定フレーム再生
					switchLoopReverse_ *= PLAY_REVERSE;
					if (switchLoopReverse_ > DEFAULT_STEP)
					{
						playAnim_.step = stepEndLoopStart_;
						playAnim_.totalTime = stepEndLoopEnd_;
					}
					else
					{
						playAnim_.step = stepEndLoopEnd_;
						playAnim_.totalTime = stepEndLoopStart_;
					}
					playAnim_.speed = endLoopSpeed_;
					
				}
				else
				{
					// 通常のループ再生
					playAnim_.step = DEFAULT_STEP;
				}
			}
			else
			{
				// ループしない
				playAnim_.step = playAnim_.totalTime;
			}

		}

	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);

}

void AnimationController::SetEndLoop(float startStep, float endStep, float speed)
{
	stepEndLoopStart_ = startStep;
	stepEndLoopEnd_ = endStep;
	endLoopSpeed_ = speed;
}

int AnimationController::GetPlayType(void) const
{
	return playType_;
}

bool AnimationController::IsEnd(void) const
{

	bool ret = false;

	if (isLoop_)
	{
		// ループ設定されているなら、
		// 無条件で終了しないを返す
		return ret;
	}

	if (playAnim_.step >= playAnim_.totalTime)
	{
		// 再生時間を過ぎたらtrue
		return true;
	}

	return ret;

}
