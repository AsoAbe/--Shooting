#include<DxLib.h>
#include "../Application.h"
#include "MessageManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

MessageManager* MessageManager::instance_ = nullptr;

void MessageManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new MessageManager();
	}
	instance_->Init();
}

MessageManager& MessageManager::GetInstance()
{
	return *instance_;
}

bool MessageManager::Init(void)
{
	//フォント作成
	messageFont_ = CreateFontToHandle("messageFont_", 24, -1);

	return true;
}

void MessageManager::Update(void)
{
	if (centerMessage_ != "")
	{
		//タイマー加算
		centerMessageTimer_++;
	}
}

void MessageManager::Draw(void)
{
	if (centerMessage_ != "")
	{
		//メッセージ全体の色
		int color = DEFAULT_MESSAGE_COLOR;
		//完全に不透明
		const int maxAlpha = 255;
		//透明度
		int alpha = maxAlpha;

		//fps込みで計算したタイム
		float messageEndTimeF = messageEndTime_ * Application::FPS;
		float messagePrintTimeF = MESSAGE_PRINT_TIME * Application::FPS;
		float messageExitTimeF = MESSAGE_EXIT_TIME * Application::FPS;
		int messageBlinkTimeF = Application::FPS / MESSAGE_BLINK;

		//文字全体の描画座標
		int posX = Application::SCREEN_SIZE_X / 2;
		int posY = Application::SCREEN_SIZE_Y / 2 + MESSAGE_OFFSET;

		if (centerMessageTimer_ < 0)
		{
			//メッセージ出現演出
			float rate = (messagePrintTimeF - fabsf(centerMessageTimer_)) / messagePrintTimeF;
			alpha = static_cast<int>(maxAlpha * rate);
			//上昇しながら現れる
			posY += static_cast<int>((1 - rate) * MESSAGE_ANIM_OFFSET);
		}
		else if (centerMessageTimer_ < messageEndTimeF)
		{
		//表示
		alpha = maxAlpha;
		}
		else if (centerMessageTimer_ < messageEndTimeF + static_cast<int>(messageExitTimeF))
		{
		//消える演出
		float rate = (centerMessageTimer_ - messageEndTimeF) / messageExitTimeF;
		alpha = maxAlpha - static_cast<int>(maxAlpha * rate);
		}
		else
		{
		//終了
		centerMessage_ = "";
		if (nextMessage_ != "")
		{
			//次のメッセージを表示
			ShowNewMessage(nextMessage_, DEFAULT_MESSAGE_TIME);
			nextMessage_ = "";
		}
		return;
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		//画面中央に合わせる
		posX -= GetDrawStringWidthToHandle(centerMessage_.c_str(), strlen(centerMessage_.c_str()), messageFont_) / 2;

		//メッセージ描画
		DrawStringToHandle(posX, posY,
			centerMessage_.c_str(), color, messageFont_);

		//マイナスを消すためにmessagePrintTimeFを足して計算
		if (((static_cast<int>(centerMessageTimer_ + messagePrintTimeF) / (Application::FPS / 4))) % 2 == 0)
		{
			//部分的に点滅させる
			color = messageColor_;
		}
		//if (messageColor_ != DEFAULT_MESSAGE_COLOR)
		//{
		//	//有効な色の場合、色付きの方角の文字を描画開始
		//	//分離したテキスト
		//	std::string dirCText = GetDirText(messageDir_, MAP_MESSAGE::dirC);
		//	std::string dirText = GetDirText(messageDir_, MAP_MESSAGE::dir);
		//	//前半の文字の分位置をずらす
		//	int cPosX = posX + GetDrawStringWidthToHandle(dirText.c_str(), strlen(dirText.c_str()), messageFont_);
		//	//色つきのテキストを上から描画
		//	DrawStringToHandle(cPosX, posY,
		//		dirCText.c_str(), color, messageFont_);
		//}

		//透明度を元に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void MessageManager::ShowNewMessage(std::string s, float time)
{
	//SoundManager::GetInstance().Play(SoundManager::SoundType::MESSAGE);
	centerMessage_ = s;
	messageColor_ = DEFAULT_MESSAGE_COLOR;
	//タイマースタート(半透明から)
	messageEndTime_ = time;
	//演出用の時間をマイナスしてタイマーをセット
	centerMessageTimer_ = static_cast<int>(Application::FPS * MESSAGE_PRINT_TIME) * -1;
}

void MessageManager::SetNextMessage(std::string s)
{
	nextMessage_ = s;
}

//void MessageManager::ShowDirMessage(Map::MAP_DIR dir, Map::MAP_MESSAGE m, int time)
//{
//	//SoundManager::GetInstance().Play(SoundManager::SoundType::MESSAGE);
//	centerMessage_ = GetDirText(dir, m);
//	messageColor_ = GetDirColor(dir);
//	if (!(m == MAP_MESSAGE::apper || m == MAP_MESSAGE::apperBoss) || dir == MAP_DIR::HIDDEN)
//	{
//		//方角関係なしのメッセージなのでデフォルトカラー
//		messageColor_ = DEFAULT_MESSAGE_COLOR;
//	}
//	messageDir_ = dir;
//	//タイマースタート(半透明から)
//	messageEndTime_ = time;
//	centerMessageTimer_ = static_cast<int>(Application::FPS * MESSAGE_PRINT_TIME) * -1;
//
//}

void MessageManager::ClearMessage()
{
	centerMessage_ = "";
	nextMessage_ = "";
}

void MessageManager::FadeoutMessage()
{
	if (centerMessage_ == "")
	{
		nextMessage_ = "";
		return;
	}
	float messageEndTimeF = messageEndTime_ * Application::FPS;
	if (centerMessageTimer_ < messageEndTimeF)
	{
		//フェードアウト開始にに時間をセット
		centerMessageTimer_ = messageEndTimeF;
	}
	//nextMessage_を表示しないように
	nextMessage_ = "";
}



MessageManager::MessageManager(void)
{
	messageFont_ = -1;
	centerMessage_ = "";
	nextMessage_ = "";
	messageColor_ = DEFAULT_MESSAGE_COLOR;
	messageEndTime_ = 0;
	centerMessageTimer_ = 0;
}

//MessageManager::MessageManager(const MessageManager& ins)
//{
//}

MessageManager::~MessageManager(void)
{
}

void MessageManager::Release(void)
{
	//フォントのメモリ解放
	DeleteFontToHandle(messageFont_);
	messageFont_ = -1;
	//インスタンス削除
	delete instance_;
	instance_ = nullptr;
}

//std::string MessageManager::GetDirText(Map::MAP_DIR dir, Map::MAP_MESSAGE m)
//{
//	std::string ret = "";
//	if (m != MAP_MESSAGE::dirC)
//	{
//		switch (dir)
//		{
//		case MAP_DIR::N:
//			ret = "北";
//			break;
//
//		case MAP_DIR::S:
//			ret = "南";
//			break;
//
//		case MAP_DIR::W:
//			ret = "西";
//			break;
//
//		case MAP_DIR::E:
//			ret = "東";
//			break;
//		case MAP_DIR::HIDDEN:
//			ret = "？";
//			break;
//		}
//	}
//	if (m == MAP_MESSAGE::dir)
//	{
//		//dirはこの部分までで終了
//		return ret;
//	}
//
//	switch (dir)
//	{
//	case MAP_DIR::N:
//		ret += "(黒色)";
//		break;
//
//	case MAP_DIR::S:
//		ret += "(赤色)";
//		break;
//
//	case MAP_DIR::W:
//		ret += "(白色)";
//		break;
//
//	case MAP_DIR::E:
//		ret += "(青色)";
//		break;
//	case MAP_DIR::HIDDEN:
//		ret += "？？";
//		break;
//	}
//
//	switch (m)
//	{
//	case MAP_MESSAGE::apper:
//		ret += "の方角から敵が出現！";
//		break;
//	case MAP_MESSAGE::text:
//		ret += "の方角から敵が出現中！";
//		break;
//	case MAP_MESSAGE::apperBoss:
//		ret += "の方角からボスが出現！";
//		break;
//	case MAP_MESSAGE::textBoss:
//		ret += "の方角からボスが出現中！";
//		break;
//	case MAP_MESSAGE::bossText:
//		//dir関係なし
//		ret = "ボスはプレイヤーを狙うので注意！";
//		break;
//	case MAP_MESSAGE::rushText:
//		//dir関係なし
//		ret = "一定時間アイテムが自動補充！";
//	}
//	return ret;
//}

//int MessageManager::GetDirColor(Map::MAP_DIR dir)
//{
//	switch (dir)
//	{
//	case MAP_DIR::N:
//		return Map::COLOR_N;
//		break;
//
//	case MAP_DIR::S:
//		return Map::COLOR_S;
//		break;
//
//	case MAP_DIR::W:
//		return Map::COLOR_W;
//		break;
//
//	case MAP_DIR::E:
//		return Map::COLOR_E;
//		break;
//	case MAP_DIR::HIDDEN:
//		return DEFAULT_MESSAGE_COLOR;
//	}
//	return -1;
//}
