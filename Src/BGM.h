#pragma once
#include <unordered_map>
#include <string>
#include<vector>
#include<functional>

enum class BgmType {
	none,//選択なし
	eMenu_locked,//ラクトガール ～ 少女密室
	eMenu_Luna,//月時計 ～ルナ・ダイアル
	eMenu_septet,//亡き王女の為のセプテット
	eMenu_UNowen,//U.N.オーエンは彼女なのか?
	eMenu_Fukanou,//不可能弾幕には反則を_アレンジ
	eMenu_Haiboku,//敗北の英雄～Somehow_survived
	eMenu_himetyumeno,//砕月～萃めた夢の欠片～

};

typedef enum BgmMenu{
	//eMenu_ReimuN,
	eMenu_locked,      //ゲーム
	eMenu_Luna,    //設定
	eMenu_septet,    //設定
	eMenu_UNowen,    //設定
	eMenu_Fukanou,
	eMenu_Haiboku,

	eMenu_BgmD,
	eMenu_Num_Bgm,        //本項目の数

} eMenuBgm;

class SceneManager;
class SceneTitle;
class BGM
{
	friend SceneTitle; //privateもいじっていいよ
private:
	int currentIndex_ = 0; // 現在選択中のインデックス
	using NameTable_t = std::unordered_map<int, std::string>;
	NameTable_t Bgm_t;//bgm用テーブル
	std::string bgmId_ = {};

	std::vector<std::string> bgmListForDisplay_;//名前テーブル

	std::string bgmName_ = {};
public:
	BGM();
	~BGM();
	void Upudat(void);
	void Draw(void);
	void ResetTable();
	void SetBgmInputType(const BgmType type);
	std::string GetBgmInputType(void);
	std::string GetBgmName();
	SceneManager* sceneManager_;
};

