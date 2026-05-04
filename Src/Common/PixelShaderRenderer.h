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
	static constexpr int NUM_VERTEX = 4;
	static constexpr int NUM_VERTEX_IDX = 6;

	PixelShaderMaterial& material_;
	// 頂点
	VERTEX2DSHADER vertexs_[NUM_VERTEX];
	// 頂点インデックス
	WORD indexes_[NUM_VERTEX_IDX];
};

