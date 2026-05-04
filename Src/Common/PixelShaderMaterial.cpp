#include "../Application.h"
#include "PixelShaderMaterial.h"

namespace
{
	/// <summary>
	/// 使用するスロット
	/// </summary>
	constexpr int SLOT = 4;
	/// <summary>
	/// FLOAT4の中のfloatの数を表す
	/// </summary>
	constexpr int RGBA = 4;
}

PixelShaderMaterial::PixelShaderMaterial(const std::string& shaderFileName, int bufSize)
{
	bufSize_ = 0;
	if (bufSize > 0)
	{
		//bufSize_は常に4の倍数
		bufSize_ = static_cast<int>(ceilf(static_cast<float>(bufSize)/ RGBA))* RGBA;
	}
	shader_ = LoadPixelShader((Application::PATH_SHADER + shaderFileName).c_str());
	bufHandle_ = CreateShaderConstantBuffer(sizeof(float) * bufSize_);
	texture_ = -1;
}

PixelShaderMaterial::~PixelShaderMaterial()
{
	int i = DeleteShaderConstantBuffer(bufHandle_);
	bufHandle_ = -1;
	DeleteShader(shader_);
	shader_ = -1;
}

void PixelShaderMaterial::SetTextrue(int tex)
{
	texture_ = tex;
}

int PixelShaderMaterial::GetTexture() const
{
	return texture_;
}

void PixelShaderMaterial::SetValue(float v, int idx)
{
	if (idx+1 > bufSize_ || idx <0)
	{
		//無効
		return;
	}
	float* bufPtr = (float*)GetBufferShaderConstantBuffer(bufHandle_);
	//ポインタを加算して動かす
	bufPtr += idx;
	*(bufPtr) = v;
}

void PixelShaderMaterial::SetColor(float r, float g, float b, float a, int idx)
{
	if (idx + RGBA > bufSize_ * RGBA || idx < 0)
	{
		//入りきらないのでリターン
		return;
	}
	float* bufPtr = (float*)GetBufferShaderConstantBuffer(bufHandle_);
	//ポインタを加算して動かす
	bufPtr += idx;
	*(bufPtr++) = r;
	*(bufPtr++) = g;
	*(bufPtr++) = b;
	*(bufPtr) = a;
}

void PixelShaderMaterial::SetColor(const COLOR_F& color, int idx)
{
	SetColor(color.r, color.g, color.b, color.a, idx);
}

int PixelShaderMaterial::GetShader() const
{
	return shader_;
}

void PixelShaderMaterial::SetShaderConstantBufferFromMaterial()
{
	//シェーダーに反映する
	UpdateShaderConstantBuffer(bufHandle_);
	SetShaderConstantBuffer(bufHandle_, DX_SHADERTYPE_PIXEL, SLOT);
}
