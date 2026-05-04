#include <DxLib.h>
#include "Application.h"
#include "Fader.h"

#include "./Common/PixelShaderMaterial.h"
#include "./Common/PixelShaderRenderer.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif
Fader::Fader(void)
{
	state_ = STATE::NONE;
	alpha_ = 0.0f;
	isPreEnd_ = true;
	isEnd_ = true;
	faderArray_.clear();
	faderScreen_ = -1;
	faderIndex_ = -1;
}

Fader::~Fader(void)
{
	DeleteGraph(faderScreen_);
	faderScreen_ = -1;
}

void Fader::Init(void)
{
	state_ = STATE::NONE;
	alpha_ = 0.0f;
	isPreEnd_ = true;
	isEnd_ = true;

	faderScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	faderArray_.push_back(PsStruct("psFade.cso"));
	faderArray_.push_back(PsStruct("psFadeRot.cso"));
}

void Fader::Update(void)
{

	if (isEnd_)
	{
		// フェード処理が終了していたら何もしない
		return;
	}
	
	if (isPreEnd_)
	{
		// 1フレーム後(Draw後)に終了とする
		isEnd_ = true;
		return;
	}

	switch (state_)
	{

	case STATE::NONE:
		return;

	case STATE::FADE_OUT:
	case STATE::FADEOUT_PS:
		alpha_ += SPEED_ALPHA;
		if (alpha_ > 255)
		{
			// フェード終了
			alpha_ = 255;
			isPreEnd_ = true;
		}
		break;

	case STATE::FADE_IN:
	case STATE::FADEIN_PS:
		alpha_ -= SPEED_ALPHA;
		if (alpha_ < 0)
		{
			// フェード終了
			alpha_ = 0;
			isPreEnd_ = true;
		}
		break;

	default:
		return;

	}

}

void Fader::Draw(int inScreen)
{

	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X,
			Application::SCREEN_SIZE_Y,
			0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	case STATE::FADEOUT_PS:
	case STATE::FADEIN_PS:;
		SetDrawScreen(faderScreen_);
		ClearDrawScreen();
		if (faderIndex_ != -1)
		{
			faderArray_[faderIndex_].psMaterial->SetValue(alpha_ / 255);
			faderArray_[faderIndex_].psRenderer->Draw(inScreen, faderScreen_);
		}
		//元に戻して内容を描画
		SetDrawScreen(inScreen);
		ClearDrawScreen();
		DrawGraph(0, 0, faderScreen_,false);
		break;
	}

}

Fader::STATE Fader::GetState(void)
{
	return state_;
}

bool Fader::IsEnd(void)const
{
	return isEnd_;
}

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
	if (state_ == STATE::FADE_IN || state_ == STATE::FADEIN_PS)
	{
		alpha_ = 255;
	}
	else if (state_ == STATE::FADE_OUT || state_ == STATE::FADEOUT_PS)
	{
		alpha_ = 0;
		faderIndex_ = GetRand(faderArray_.size() - 1);
	}
}

Fader::PsStruct::PsStruct(const std::string& name)
{
	psMaterial = std::make_unique<PixelShaderMaterial>(name.c_str(), 1);
	psRenderer = std::make_unique<PixelShaderRenderer>(*psMaterial);
	psRenderer->MakeScreenSquereVertex();
}
