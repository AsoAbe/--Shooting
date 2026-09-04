#include<DxLib.h>
#include "BGM.h"
#include "Application.h"
#include "../Src/Manager/SceneManager.h"

void 
BGM::SetBgmInputType(const BgmType tyep)
{

	switch (tyep)
	{
	case BgmType::eMenu_locked:
		bgmId_ = (Application::PATH_BGM + "GameBgm/Touhou_locked-girl_YouFulca.mp3").c_str();
		bgmName_ = { "ƒ‰ƒNƒgƒK[ƒ‹ ` ­—–§º" };
		break;
	case BgmType::eMenu_Luna:
		bgmId_ = (Application::PATH_BGM + "GameBgm/Touhou_Luna-dial_YouFulca.mp3").c_str();
		bgmName_ = { "ŒŒv `ƒ‹ƒiEƒ_ƒCƒAƒ‹" };
		break;
	case BgmType::eMenu_septet:
		bgmId_ = (Application::PATH_BGM + "GameBgm/Touhou_septet-YouFulca.mp3").c_str();
		bgmName_ = { "–S‚«‰¤—‚Ìˆ×‚ÌƒZƒvƒeƒbƒg" };
		break;
	case BgmType::eMenu_UNowen:
		bgmId_ = (Application::PATH_BGM + "GameBgm/Touhou_UNowen-YouFulca.mp3").c_str();
		bgmName_ = { "U.N.ƒI[ƒGƒ“‚Í”Ş—‚È‚Ì‚©?" };
		break;
	
	case BgmType::eMenu_Fukanou:
		bgmId_ = (Application::PATH_BGM + "GameBgm/nc132382_y“Œ•ûz•s‰Â”\’e–‹‚É‚Í”½‘¥‚ğ_ƒAƒŒƒ“ƒW.mp3").c_str();
		bgmName_ = { "•s‰Â”\’e–‹‚É‚Í”½‘¥‚ğ_ƒAƒŒƒ“ƒW" };
		break;

	case BgmType::eMenu_Haiboku:
		bgmId_ = (Application::PATH_BGM + "GameBgm/nc138201_”s–k‚Ì‰p—Y`Somehow_survived.mp3").c_str();
		bgmName_ = { "”s–k‚Ì‰p—Y`Somehow_survived" };
		break;

	case BgmType::eMenu_himetyumeno:
		bgmId_ = (Application::PATH_BGM + "GameBgm/nc378681_y“Œ•ûƒAƒŒƒ“ƒWzÓŒ`äÂ‚ß‚½–²‚ÌŒ‡•Ğ`y“Œ•û“®‰æBGMx‰‡z.wav").c_str();
		bgmName_ = { "ÓŒ`äÂ‚ß‚½–²‚ÌŒ‡•Ğ`" };
		break;

	case BgmType::none:
		bgmId_ = Application::PATH_BGM + "GameBgm/Touhou_locked-girl_YouFulca.mp3";
		bgmName_ = { "ƒ‰ƒNƒgƒK[ƒ‹ ` ­—–§º" };
		break;
	default:
		
		break;
	}
}

std::string BGM::GetBgmInputType(void)
{
	return bgmId_;
}

std::string BGM::GetBgmName()
{
	return bgmName_;
}

BGM::BGM()
{
	bgmListForDisplay_ = {
		"ƒ‰ƒNƒgƒK[ƒ‹","ƒ‹ƒiƒeƒBƒbƒN"
	};
	Bgm_t[(int)BgmType::eMenu_locked] = "ƒ‰ƒNƒgƒK[ƒ‹ ` ­—–§º";
	
	bgmId_ = "";
	bgmName_ = "";
}

BGM::~BGM()
{
	bgmId_ = "";
	bgmName_ = "";
}

void BGM::Upudat(void)
{
}

void BGM::Draw(void)
{
}

void BGM::ResetTable()
{
}
