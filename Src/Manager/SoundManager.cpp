#include <DxLib.h>
#include "../Application.h"
#include "SoundManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

SoundManager* SoundManager::instance_ = nullptr;

#define PATH_CURSOR "カーソル移動2.mp3"
#define PATH_GRAZE "カーソル移動2.mp3"
#define PATH_SHOT "ナイフを投げる.mp3"
#define PATH_SHOT_HIGH "ビーム砲1.mp3"
#define PATH_HIT "軽いパンチ1.mp3"
#define PATH_HIT_HIGH "軽いパンチ1.mp3"
#define PATH_HIT_PLAYER "ナイフで切る.mp3"
#define PATH_DEATH_PLAYER "爆発1.mp3"
#define PATH_SUCCESS "決定ボタンを押す53.mp3"
#define PATH_JUMP "キックの素振り3.mp3"
#define PATH_CHARGE "聖魔法.mp3"
#define PATH_WIN "聖魔法.mp3"
#define PATH_ENEMY_DASH "爆発1.mp3"

#define PATH_BGM_TITLE "aaa.mp3"

SoundManager::SoundData::SoundData(int handle, int volume)
{
	this->handle = handle;
	this->volume = volume;
	this->ct = 0;
}

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
}

SoundManager& SoundManager::GetInstance()
{
    return *instance_;
}


void SoundManager::Update(void)
{
	//fpsから1フレームの時間を入力する
	CtCountDown(1.0f / Application::FPS);
}

void SoundManager::Draw(void)
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "LoadedSoundCount = %d", loadedSound_.size());
#endif
}

void SoundManager::Release(void)
{
	//インスタンス削除
	delete instance_;
	instance_ = nullptr;
}

void SoundManager::CtCountDown(float deltaTime)
{
	//クールタイム経過
	for (auto& s : loadedSound_)
	{
		if (s.second.ct > 0)
		{
			s.second.ct -= deltaTime;
		}
		else
		{
			s.second.ct = 0;
		}
		
	}
}

void SoundManager::ReleaseAllSound(void)
{
	for (auto& m : loadedSound_)
	{
		//サウンドハンドルを取り出してメモリ開放
		DeleteSoundMem(m.second.handle);
	}
	loadedSound_.clear();
	playingBgm_ = SOUND_ID::NONE;
	LoadSound(SOUND_ID::NONE);
}

void SoundManager::LoadSound(SOUND_ID sound, int volume)
{
	if (IsLoaded(sound))
	{
		//既に存在する
		return;
	}
	int handle = -1;
	if (sound != SOUND_ID::NONE)
	{
		handle = LoadSoundMem(GetSoundPath(sound).c_str());
		if (handle != -1)
		{
			ChangeVolumeSoundMem(volume, handle);
		}
	}
	loadedSound_.emplace(sound,SoundData(handle, volume));
}

void SoundManager::PlaySE(SOUND_ID sound)const
{
	if (IsLoaded(sound))
	{
		PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_BACK);
	}
}

void SoundManager::PlaySE(SOUND_ID sound, bool load, int volume)
{
	if (IsLoaded(sound))
	{
		//音量変更
		ChangeSoundVolume(sound, volume);
		PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_BACK);
	}
	else if (load)
	{
		LoadSound(sound,volume);
		//ロードしてから鳴らす
		if (IsLoaded(sound))
		{
			PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_BACK);
		}
	}
}

void SoundManager::PlaySE_CT(SOUND_ID sound, float ct)
{
	if (IsLoaded(sound))
	{
		SoundData& soundData = loadedSound_.at(sound);
		if (soundData.ct > 0)
		{
			//ct中なので無効
			return;
		}
		//サウンドを再生してctをセット
		PlaySoundMem(soundData.handle, DX_PLAYTYPE_BACK);
		soundData.ct = ct;
	}
}

void SoundManager::ChangeBGM(SOUND_ID sound, bool load, int volume)
{
	const SoundData* plaingData = GetSoundData(playingBgm_);
	if (plaingData != nullptr)
	{
		if (plaingData->handle != -1)
		{
			StopSoundMem(plaingData->handle);
		}
	}
	if (sound == SOUND_ID::NONE)
	{
		//サウンド終了時点でリターン
		return;
	}


	if (IsLoaded(sound))
	{
		//音量変更
		ChangeSoundVolume(sound, volume);
		PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_LOOP);
	}
	else if (load)
	{
		LoadSound(sound, volume);
		//ロードしてから鳴らす
		if (IsLoaded(sound))
		{
			PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_LOOP);
		}
	}
}

bool SoundManager::IsLoaded(SOUND_ID sound) const
{
	return (loadedSound_.find(sound) != loadedSound_.end());
}

void SoundManager::SetActiveBGM(bool b)
{
	isActiveBGM_ = b;
}

bool SoundManager::IsActiveBGM() const
{
	return isActiveBGM_;
}

int SoundManager::GetBgmVolume() const
{
	return isActiveBGM_ ? VOLUME_BGM : 0;
}

void SoundManager::ChangeVolumeSoundMem_bgm(int volume, int handle)
{
	ChangeVolumeSoundMem(isActiveBGM_ ? volume : 0, handle);
}

int SoundManager::GetBgmVolume(int volume) const
{
	return isActiveBGM_ ? volume : 0;
}

void SoundManager::StopSound(SOUND_ID sound) const
{
	if (GetSoundData(sound) == nullptr)
	{
		//該当なし
		return;
	}
	StopSoundMem(GetSoundData(sound)->handle);
}

SoundManager::SoundManager(void)
{
	playingBgm_ = SOUND_ID::NONE;
	loadedSound_.clear();
	LoadSound(SOUND_ID::NONE);
	isActiveBGM_ = true;
}

SoundManager::~SoundManager(void)
{
	for (auto& m : loadedSound_)
	{
		//サウンドハンドルを取り出してメモリ開放
		DeleteSoundMem(m.second.handle);
	}
	loadedSound_.clear();
}

std::string SoundManager::GetSoundPath(SOUND_ID sound)
{
	//constexpr std::string s = ";";

	std::string ret = Application::PATH_SOUND;
	switch (sound)
	{
	case SOUND_ID::CURSOR:
		ret += PATH_CURSOR;
		break;
	case SOUND_ID::GRAZE:
		ret += PATH_GRAZE;
		break;
	case SOUND_ID::DEATH_PLAYER:
		ret += PATH_DEATH_PLAYER;
		break;
	case SoundManager::SOUND_ID::SHOT:
		ret += PATH_SHOT;
		break;
	case SoundManager::SOUND_ID::SHOT_HIGH:
		ret += PATH_SHOT_HIGH;
		break;
	case SoundManager::SOUND_ID::HIT:
		ret += PATH_HIT;
		break;
	case SoundManager::SOUND_ID::HIT_HIGH:
		ret += PATH_HIT_HIGH;
		break;
	case SoundManager::SOUND_ID::HIT_PLAYER:
		ret += PATH_HIT_PLAYER;
		break;
	case SoundManager::SOUND_ID::SUCCESS:
		ret += PATH_SUCCESS;
		break;
	case SoundManager::SOUND_ID::JUMP:
		ret += PATH_JUMP;
		break;
	case SoundManager::SOUND_ID::CHARGE:
		ret += PATH_CHARGE;
		break;
	case SoundManager::SOUND_ID::WIN:
		ret += PATH_WIN;
		break;
	case SoundManager::SOUND_ID::ENEMY_DASH:
		ret += PATH_ENEMY_DASH;
		break;

	case SOUND_ID::BGM_TITLE:
		ret += PATH_BGM_TITLE;
		break;

	case SoundManager::SOUND_ID::BOSS:
	case SoundManager::SOUND_ID::NONE:
	default:
		ret = "";
		break;
	}

	return ret;
}

const SoundManager::SoundData* SoundManager::GetSoundData(SOUND_ID sound) const
{
	if (!(IsLoaded(sound)))
	{
		return nullptr;
	}
	return &loadedSound_.at(sound);
}

void SoundManager::ChangeSoundVolume(SOUND_ID sound, int volume)
{
	if (!(IsLoaded(sound)))
	{
		return ;
	}
	int& volumeData = loadedSound_.at(sound).volume;
	const int& handle = loadedSound_.at(sound).handle;
	if (volumeData == volume || handle < 0)
	{
		//書き換えなし
		return;
	}
	//書き換えて反映
	volumeData = volume;
	ChangeVolumeSoundMem(volumeData, handle);
}

