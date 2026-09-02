//#pragma once
#include<string>
//前方宣言
class Fader;
class SceneBase;
class Camera;

class SceneManager
{
public:
	// 無効な画像・スクリーンハンドル
	static constexpr int INVALID_GRAPH_HANDLE = -1;

	// 背景色
	static constexpr int BACKGROUND_COLOR_R = 138;
	static constexpr int BACKGROUND_COLOR_G = 190;
	static constexpr int BACKGROUND_COLOR_B = 222;

	// グローバルアンビエントライトの強さ
	static constexpr float AMBIENT_LIGHT_R = 0.2f;
	static constexpr float AMBIENT_LIGHT_G = 0.2f;
	static constexpr float AMBIENT_LIGHT_B = 0.2f;
	static constexpr float AMBIENT_LIGHT_A = 1.0f;

	// ディレクショナルライトの方向
	static constexpr float LIGHT_DIRECTION_X = 0.0f;
	static constexpr float LIGHT_DIRECTION_Y = -1.0f;
	static constexpr float LIGHT_DIRECTION_Z = 1.0f;

	// フォグの色
	static constexpr int FOG_COLOR_R = 110;
	static constexpr int FOG_COLOR_G = 152;
	static constexpr int FOG_COLOR_B = 178;

	// フォグの開始・終了距離
	static constexpr float FOG_START_DISTANCE = 8000.0f;
	static constexpr float FOG_END_DISTANCE = 10000.0f;
	//列挙型
	//-----

	//シーン管理
	enum class SCENE_ID
	{
		NONE
		,TITLE
		,GAME
		,GAMEOVER
		,TUTORIAL
	};

	//メンバー関数
	//-----
	
	//インスタンスの生成(シングルトン)(外部から静的にインスタンスを生成)
	static void CreateInstance(void);
	//インスタンスを返す
	//参照型にしているがポインタ型でもよい
	static SceneManager& GetInstance(void);

	//初期化
	bool Init(void);
	//更新
	void Update(void);
	//描画
	void Draw(void);
	//解放
	bool Release(void);

	//シーン変更を依頼
	//isToFadeがtrueでフェードアウト
	void ChangeScene(SCENE_ID nextID ,bool IsToFade);

	Camera* GetCamera();

	//スクリーンのハンドルを返す
	int GetMainScreen()const;

	//フェードの終了状態を返す
	bool IsEndFade(void)const;

	void BgmChang(std::string name);
	std::string GetBgmChangType(void);
private:
	//int testPlayingEffect_;
	//int testEffect_;

	int backgroundColor_;//背景の色;
	int testImg_;//Effekseer用の画像

	Camera* camera_;
	//静的インスタンス
	static SceneManager* instance_;
	//コンストラクタ
	SceneManager(void);
	//コピーのコンストラクタも潰す(privateに)
	SceneManager(const SceneManager& ins);

	//デストラクタ
	~SceneManager(void);

	void Destroy(void);//インスタンスを破棄

	void Init3D();
	
	//メンバー変数
	//-----
	SCENE_ID sceneID_;//現在のシーン
	SCENE_ID waitSceneID_;//次に遷移するシーン
	bool isSceneChanging_;//シーン遷移中かの判断(true 遷移中)

	
	std::string bgmChang_;
	//フェード用
	Fader* fader_;//フェーダーのインスタンス
	//シーン
	SceneBase* scene_;//シーン格納

	int mainScreen_;

	//メンバー関数
	//-----
	
	//フェード実施用
	void Fade(void);
	//シーンを切り替える
	void DoChangeScene(void);
	//シーンの解放処理
	void ReleaseScene(void);
};