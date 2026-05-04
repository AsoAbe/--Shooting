#include "../Application.h"
#include "../Common/AsoUtility.h"
#include "PixelShaderMaterial.h"
#include "PixelShaderRenderer.h"

PixelShaderRenderer::PixelShaderRenderer(PixelShaderMaterial& material):material_(material)
{
	for (auto& i : indexes_)
	{
		i = -1;
	}
	for (auto& v : vertexs_)
	{
		v = {};
	}
}

PixelShaderRenderer::~PixelShaderRenderer()
{
}

void PixelShaderRenderer::Draw(int inScreen, int outScreen)
{
	if (indexes_[0] < 0)
	{
		//まだMakeSquereVertexされていない
		return;
	}
	//シェーダー使用開始
	MV1SetUseOrigShader(true);
	// 使用するテクスチャを0番にセット
	material_.SetTextrue(inScreen);
	//定数バッファ反映
	material_.SetShaderConstantBufferFromMaterial();
	SetUseTextureToShader(0, material_.GetTexture());

	// 使用するピクセルシェーダーをセット
	SetUsePixelShader(material_.GetShader());
	SetDrawScreen(outScreen);
	DrawPolygonIndexed2DToShader(vertexs_, NUM_VERTEX, indexes_, NUM_VERTEX_IDX);

	//解除
	MV1SetUseOrigShader(false);
	SetUseTextureToShader(0, -1);
	SetUsePixelShader(-1);
}

void PixelShaderRenderer::SetPosAndDraw(int inScreen, int outScreen, int x, int y)
{
	if (indexes_[0] < 0)
	{
		//まだMakeSquereVertexされていない
		return;
	}
	//目標座標との差
	VECTOR diff = VSub(VGet(x,y,0), vertexs_[0].pos);

	//0でも問題ないと思うけど念のため
	if (VSquareSize(diff) > 1.0f)
	{
		for (auto& vp : vertexs_)
		{
			//頂点を移動
			vp.pos = VAdd(vp.pos, diff);
		}
	}
	Draw(inScreen, outScreen);
}

void PixelShaderRenderer::MakeSquereVertex(int sx, int sy, int ex, int ey)
{
	// ４頂点の初期化
	for (int i = 0; i < NUM_VERTEX; i++)
	{
		vertexs_[i].pos = VGet(0, 0, 0);
		vertexs_[i].u = 0;
		vertexs_[i].v = 0;
		vertexs_[i].rhw = 1.0f;
		vertexs_[i].dif = GetColorU8(255, 255, 255, 255);
		vertexs_[i].spc = GetColorU8(255, 255, 255, 255);
		vertexs_[i].su = 0.0f;
		vertexs_[i].sv = 0.0f;
	}
	//左上
	vertexs_[0].pos = VGet(sx, sy, 0);
	vertexs_[0].u = 0;
	vertexs_[0].v = 0;
	//右上
	vertexs_[1].pos = VGet(ex, sy, 0);
	vertexs_[1].u = 1;
	vertexs_[1].v = 0;
	//左下
	vertexs_[2].pos = VGet(sx, ey, 0);
	vertexs_[2].u = 0;
	vertexs_[2].v = 1;
	//右下
	vertexs_[3].pos = VGet(ex, ey, 0);
	vertexs_[3].u = 1;
	vertexs_[3].v = 1;

	//インデックス登録
	indexes_[0] = 2;
	indexes_[1] = 0;
	indexes_[2] = 1;
	indexes_[3] = 1;
	indexes_[4] = 3;
	indexes_[5] = 2;
}

void PixelShaderRenderer::MakeScreenSquereVertex()
{
	MakeSquereVertex(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);
}
