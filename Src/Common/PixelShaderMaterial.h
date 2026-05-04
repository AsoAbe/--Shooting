#pragma once
#include <DxLib.h>
#include<string>

class PixelShaderMaterial
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="shaderFileName">ロードに使用するファイル名</param>
	/// <param name="bufSize">バッファに使用するfloatの数。4の倍数以外でも4の倍数として扱う</param>
	PixelShaderMaterial(const std::string& shaderFileName,int bufSize);

	~PixelShaderMaterial();
	
	void SetTextrue(int tex);
	int GetTexture()const;

	/// <summary>
	/// 定数バッファに値を入れて更新する。
	/// </summary>
	/// <param name="v">値</param>
	/// <param name="idx">インデックス。格納しきれない場合は無視</param>
	void SetValue(float v,int idx=0);

	/// <summary>
	/// 定数バッファに値を入れて更新する。(rgba)
	/// </summary>
	/// <param name="r"></param>
	/// <param name="g"></param>
	/// <param name="b"></param>
	/// <param name="a"></param>
	/// <param name="idx">頭のインデックス。1つでも格納できない値がある場合は無視</param>
	void SetColor(float r, float g, float b, float a, int idx = 0);
	/// <summary>
	/// 定数バッファに値を入れて更新する。(rgba)
	/// </summary>
	/// <param name="color"></param>
	/// <param name="idx">頭のインデックス。1つでも格納できない値がある場合は無視</param>
	void SetColor(const COLOR_F& color, int idx = 0);

	int GetShader()const;

	/// <summary>
	/// バッファをシェーダーに反映する。
	/// </summary>
	void SetShaderConstantBufferFromMaterial();
private:
	//グラフィックハンドル
	int texture_;
	//シェーダーのハンドル
	int shader_;

	//バッファに格納できるFLOAT4の数。1増えるごとにfloatが4つ
	int bufSize_;

	//バッファのハンドル
	int bufHandle_;
};

