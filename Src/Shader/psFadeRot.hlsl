struct PS_INPUT
{
	float4 svPos    : SV_POSITION;
	float4 diffuse  : COLOR0;    //拡散反射の色
	float2 uv       : TEXCOORD0;      //UV値
	float2 suv      : TEXCOORD1;     //サブテクスチャのUV値
};

Texture2D tex : register(t0);               //テクスチャ
SamplerState texSampler : register(s0);     //サンプラー

cbuffer buffer:register(b4)
{
	float value;
}

float4 main(PS_INPUT psInput) : SV_TARGET
{
	//1920 / 1080の割り算
	const float ScreenSizeRateXY =1.7777;
	const float ScreenSizeRateYX = 0.5625;

	//アニメーション速度(整数以外だと不自然になる)
	const float Spd = 2;

	const float PI = 3.1415;
	//float v = sin(clamp(fmod(value, 2), 0.0f, 1.0f) * PI * 0.5f);
	float v = value;
	float rad = v*(PI * 2)* Spd;
	//シェーダーに渡したテクスチャからuv座標をもとに場所を特定し、
	//その場所の色を取得する(サンプリング)
	

	//画像の中心を0.5fとして計算する
	float2 inUV = psInput.uv;
	inUV.x += -0.5f;
	inUV.y += -0.5f;
	float2 outUV;
	outUV.x = (inUV.x* cos(rad))- (inUV.y* ScreenSizeRateYX * sin(rad));
	outUV.y = (inUV.x* ScreenSizeRateXY * sin(rad)) + (inUV.y * cos(rad));

	//徐々に中心に寄せる
	outUV.x *= (1 - v) > 0 ? 1.0f / (1 - v) : 1;
	outUV.y *= (1 - v) > 0 ? 1.0f / (1 - v) : 1;

	//0を左上に戻す
	outUV.x += 0.5f;
	outUV.y += 0.5f;
	
	//範囲外なら黒を描画
	float4 col = (outUV.x >1 || outUV.x <0) || (outUV.y > 1 || outUV.y < 0) ? float4(0,0,0,1): tex.Sample(texSampler, outUV);

	return float4(lerp(col.r, 1, v), lerp(col.g, 1, v), lerp(col.b, 1, v), 1.0f);;
}