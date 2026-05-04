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

	const float power = 0.4f;
	const float loop = 8;
	const float PI = 3.1415;
	//float v = sin(clamp(fmod(value, 2), 0.0f, 1.0f) * PI * 0.5f);
	float v = value;
	//シェーダーに渡したテクスチャからuv座標をもとに場所を特定し、
	//その場所の色を取得する(サンプリング)
	float2 newUV = psInput.uv;
	float y = (psInput.uv.y + v);
	newUV.x = clamp(newUV.x +  sin(y* PI* loop)* v * power,0,1);

	float4 srcCol = tex.Sample(texSampler, newUV);

	return float4(lerp(srcCol.r, 1, v), lerp(srcCol.g, 1, v), lerp(srcCol.b, 1, v), 1.0f);
}