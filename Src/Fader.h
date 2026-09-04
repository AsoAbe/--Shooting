#pragma once
#include<memory>
#include<vector>

class PixelShaderMaterial;
class PixelShaderRenderer;

class Fader
{

public:
	// 初期値
	static constexpr float INITIAL_ALPHA = 0.0f;
	static constexpr int INVALID_HANDLE = -1;

	// 透明度
	static constexpr float MIN_ALPHA = 0.0f;
	static constexpr float MAX_ALPHA = 255.0f;

	// 描画
	static constexpr int DRAW_POS_START = 0;
	static constexpr int DRAW_BLEND_RESET = 0;
	static constexpr int DRAW_COLOR_BLACK = 0x000000;

	// フェーダー選択
	static constexpr int RANDOM_INDEX_OFFSET = 1;

	// ピクセルシェーダーマテリアルのテクスチャ数
	static constexpr int MATERIAL_TEXTURE_COUNT = 1;

	// フェーダー配列のインデックス調整値
	static constexpr int FADER_INDEX_OFFSET = 1;

	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 5;

	// 状態
	enum class STATE
	{
		NONE, 
		FADE_OUT,	// 徐々に暗転
		FADE_IN,	// 徐々に明転
		FADEOUT_PS,	// ピクセルシェーダーを使ってFADE_OUT
		FADEIN_PS	// ピクセルシェーダーを使ってFADE_IN
	};

	// コンストラクタ
	Fader(void);

	// デストラクタ
	~Fader(void);

	void Init(void);
	void Update(void);
	void Draw(int inScreen);

	// 状態の取得
	STATE GetState(void);

	// フェード処理が終了しているか
	bool IsEnd(void)const;

	// 指定フェードを開始する
	void SetFade(STATE state);

private:

	struct PsStruct
	{
	public:
		std::unique_ptr<PixelShaderMaterial> psMaterial;
		std::unique_ptr< PixelShaderRenderer> psRenderer;
		PsStruct(const std::string& name);
	};

	// 状態
	STATE state_;

	//フェード用のスクリーンハンドル
	int faderScreen_;

	std::vector<PsStruct> faderArray_;

	/// <summary>
	/// 使用中のフェーダー
	/// </summary>
	int faderIndex_;

	// 透明度
	float alpha_;

	// 状態(STATE)を保ったまま終了判定を行うため、
	// Update->Draw->Updateの1フレーム判定用
	bool isPreEnd_;

	// フェード処理の終了判定
	bool isEnd_;

};
