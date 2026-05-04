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
	float power;
}

float4 main(PS_INPUT psInput) : SV_TARGET
{
	const float PI = 3.1415;
	//シェーダーに渡したテクスチャからuv座標をもとに場所を特定し、
	//その場所の色を取得する(サンプリング)
	float4 srcCol = tex.Sample(texSampler,psInput.uv);

	float v = sin(clamp(fmod(value, 2), 0.0f, 1.0f) * PI * 0.5f);

	return float4(lerp(srcCol.r, 1, v* power), lerp(srcCol.g, 1, v* power), lerp(srcCol.b, 1, v* power), srcCol.a);
}