#pragma once

#include <string>
#include "../Common/Vector2.h"
class MessageManager
{
public:

	//メッセージの基本カラー
	static constexpr int DEFAULT_MESSAGE_COLOR = 0xFFFF00;
	//デフォルト表示タイム(秒)
	static constexpr float DEFAULT_MESSAGE_TIME = 2.5f;
	//表示されるまでの演出時間(秒)
	static constexpr float MESSAGE_PRINT_TIME = 0.25f;
	//完全に消えるまでの演出時間(秒)
	static constexpr float MESSAGE_EXIT_TIME = 0.25f;
	//点滅(fpsからの割り算)
	static constexpr int MESSAGE_BLINK = 8;

	//メッセージの画面中央からのオフセット(y座標)
	static constexpr float MESSAGE_OFFSET = -128;
	//演出のオフセット位置
	static constexpr float MESSAGE_ANIM_OFFSET = 16;


	//インスタンスの生成(シングルトン)(外部から静的にインスタンスを生成)
	static void CreateInstance(void);
	//インスタンスを返す
	static MessageManager& GetInstance();

	//初期化
	bool Init(void);

	//毎フレーム実行処理
	void Update(void);
	//描画処理
	void Draw(void);

	//メモリ解放してインスタンスを破棄
	void Release(void);

	//文章と秒数をセットする
	void ShowNewMessage(std::string s,float time);

	//セットしたメッセージの次に表示されるメッセージ
	void SetNextMessage(std::string s);

	//void ShowDirMessage(Map::MAP_DIR dir, Map::MAP_MESSAGE m, int time);

	//メッセージ全削除(シーン遷移)
	void ClearMessage();
	//メッセージを即フェードアウトに切り替え(画面遷移など)
	void FadeoutMessage();

	////方向からテキストを返す
	//static std::string GetDirText(Map::MAP_DIR dir, Map::MAP_MESSAGE m);
	////方向から色を返す
	//static int GetDirColor(MAP_DIR dir);



private:
	//インスタンス
	static MessageManager* instance_;

	int messageFont_;//中央表示される文字
	std::string centerMessage_;//中央に表示する文章 内容が""のときは表示なし
	std::string nextMessage_;//現在のメッセージ終了後に次に表示されるメッセージ 内容が""のときは表示なし
	//timerがendtimeに達すると消え始めるに,0以下で半透明
	float messageEndTime_;
	int centerMessageTimer_;
	//colorの色で点滅
	int messageColor_;
	////方角を記録
	//Map::MAP_DIR messageDir_;


	//コンストラクタ
	MessageManager(void);
	//コピーのコンストラクタも潰す(privateに)
	MessageManager(const MessageManager& ins) = delete;

	//デストラクタ
	~MessageManager(void);

};

