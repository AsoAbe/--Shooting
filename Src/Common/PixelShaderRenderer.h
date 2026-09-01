#pragma once
#include <DxLib.h>

class PixelShaderMaterial;

class PixelShaderRenderer
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="material">使用するマテリアルクラス</param>
	PixelShaderRenderer(PixelShaderMaterial& material);
	~PixelShaderRenderer();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="inScreen">テクスチャとして扱うスクリーンハンドル(グラフィック)</param>
	/// <param name="outScreen">描画先のスクリーンハンドル(ClearDrawScreenはこの関数内ではやらない)</param>
	void Draw(int inScreen,int outScreen);

	/// <summary>
	/// 頂点を移動させた上で描画する。
	/// </summary>
	/// <param name="inScreen">テクスチャとして扱うスクリーンハンドル(グラフィック)</param>
	/// <param name="outScreen">描画先のスクリーンハンドル(ClearDrawScreenはこの関数内ではやらない)</param>
	/// <param name="x">右上の座標X</param>
	/// <param name="y">右上の座標Y</param>
	void SetPosAndDraw(int inScreen, int outScreen,int x,int y);

	/// <summary>
	/// 描画用のポリゴンのための頂点を設定する
	/// </summary>
	/// <param name="sx">左上のX座標</param>
	/// <param name="sy">左上のY座標</param>
	/// <param name="ex">右下のX座標</param>
	/// <param name="ey">右下のY座標</param>
	void MakeSquereVertex(int sx, int sy, int ex, int ey);

	/// <summary>
	/// スクリーンサイズに合わせて描画用のポリゴンのための頂点を設定する。
	/// </summary>
	void MakeScreenSquereVertex();
private:
	// 頂点数
	static constexpr int NUM_VERTEX = 4;

	// 頂点インデックス数
	static constexpr int NUM_VERTEX_IDX = 6;

	// 無効なインデックス
	static constexpr int INVALID_INDEX = -1;

	// テクスチャを設定するスロット番号
	static constexpr int TEXTURE_SLOT = 0;

	// 頂点の初期座標
	static constexpr float DEFAULT_POSITION = 0.0f;

	// テクスチャ座標の開始位置
	static constexpr float TEXTURE_START = 0.0f;

	// テクスチャ座標の終了位置
	static constexpr float TEXTURE_END = 1.0f;

	// 頂点の深度
	static constexpr float DEFAULT_RHW = 1.0f;

	// 頂点カラー
	static constexpr int DEFAULT_COLOR = 255;

	// 座標移動が不要と判断する最小距離
	static constexpr float MIN_MOVE_DISTANCE = 1.0f;

	// 頂点番号
	static constexpr int VERTEX_LEFT_TOP = 0;
	static constexpr int VERTEX_RIGHT_TOP = 1;
	static constexpr int VERTEX_LEFT_BOTTOM = 2;
	static constexpr int VERTEX_RIGHT_BOTTOM = 3;

	PixelShaderMaterial& material_;
	// 頂点
	VERTEX2DSHADER vertexs_[NUM_VERTEX];
	// 頂点インデックス
	WORD indexes_[NUM_VERTEX_IDX];
};

