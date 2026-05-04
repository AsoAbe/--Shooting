#pragma once
#include <string>

//前方宣言
class SceneManager;

struct Size
{
	int width;//幅
	int height;//高さ
};
class Application
{
public:
	//定数
	//-----
	static constexpr int SCREEN_SIZE_X = 1280;//画面サイズx800
	static constexpr int SCREEN_SIZE_Y = 720;//画面サイズy600
	static constexpr int MAINGAME_SIZE_X = 720;//メインゲームのスクリーンサイズ
	static constexpr int MAINGAME_POS_X = 280;//メインゲームを中央に寄せる

	//60FPS基準で設定された定数値を調整する用の60
	static constexpr int DEFAULT_FPS = 60;//こっちは変更禁止
	//基準fps
	static constexpr int FPS = 60;
	//キー長押し判定時間(秒)
	static constexpr float LONG_KEY_DOWN_TIME_S = 1.0f;

	//1フレーム当たりの秒数
	static constexpr float FRAME_RATE = (1000.0f / FPS);


	static const std::string PATH_MODEL;
	static const std::string PATH_IMAGE;
	static const std::string PATH_EFFECT;
	static const std::string PATH_SOUND;
	static const std::string PATH_STAGE;
	static const std::string PATH_SHADER;
	static const std::string PATH_FONT;
	static const std::string PATH_BGM;
	//メンバー関数
	//-----
	//インスタンスの生成(シングルトン)(外部から静的にインスタンスを生成)
	static void CreateInstance(void);
	//インスタンスを返す
	static Application& GetInstance(void);

	const Size& GetWindowSize()const;
	//初期化
	bool Init(void);
	//ゲームループ処理
	void Run(void);
	//解放
	bool Release(void);

private:
	//メンバー変数
	//-----
	//静的インスタンス
	static Application* instance_;

	//fps管理
	int time_;
	int lastFrameTime_;

	int frameCount_;
	int fpsUpdateTime_;
	float fps_;
	Size windoSize_ = { 640,480 };
	

	//コンストラクタ
	Application(void);
	//コピーのコンストラクタも潰す(privateに)
	Application(const Application& ins);

	//デストラクタ
	~Application(void);

	void Destroy(void);//インスタンスを破棄


};