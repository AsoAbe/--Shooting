#include "../Application.h"
#include "../Common/AsoUtility.h"
#include "PixelShaderMaterial.h"
#include "PixelShaderRenderer.h"

PixelShaderRenderer::PixelShaderRenderer(PixelShaderMaterial& material):material_(material)
{
	for (auto& i : indexes_)
	{
		i = INVALID_INDEX;
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
	SetUseTextureToShader(TEXTURE_SLOT, material_.GetTexture());

	// 使用するピクセルシェーダーをセット
	SetUsePixelShader(material_.GetShader());
	SetDrawScreen(outScreen);
	DrawPolygonIndexed2DToShader(vertexs_, NUM_VERTEX, indexes_, NUM_VERTEX_IDX);

	//解除
	MV1SetUseOrigShader(false);
	SetUseTextureToShader(TEXTURE_SLOT, INVALID_INDEX);
	SetUsePixelShader(INVALID_INDEX);
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
	if (VSquareSize(diff) > MIN_MOVE_DISTANCE)
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
		vertexs_[i].pos = VGet(
			DEFAULT_POSITION,
			DEFAULT_POSITION,
			DEFAULT_POSITION);

		vertexs_[i].u = TEXTURE_START;
		vertexs_[i].v = TEXTURE_START;
		vertexs_[i].rhw = DEFAULT_RHW;

		vertexs_[i].dif = GetColorU8(
			DEFAULT_COLOR,
			DEFAULT_COLOR,
			DEFAULT_COLOR,
			DEFAULT_COLOR);

		vertexs_[i].spc = GetColorU8(
			DEFAULT_COLOR,
			DEFAULT_COLOR,
			DEFAULT_COLOR,
			DEFAULT_COLOR);

		vertexs_[i].su = TEXTURE_START;
		vertexs_[i].sv = TEXTURE_START;
	}
	// 左上
	vertexs_[0].pos = VGet(sx, sy, DEFAULT_POSITION);
	vertexs_[0].u = TEXTURE_START;
	vertexs_[0].v = TEXTURE_START;

	// 右上
	vertexs_[1].pos = VGet(ex, sy, DEFAULT_POSITION);
	vertexs_[1].u = TEXTURE_END;
	vertexs_[1].v = TEXTURE_START;

	// 左下
	vertexs_[2].pos = VGet(sx, ey, DEFAULT_POSITION);
	vertexs_[2].u = TEXTURE_START;
	vertexs_[2].v = TEXTURE_END;

	// 右下
	vertexs_[3].pos = VGet(ex, ey, DEFAULT_POSITION);
	vertexs_[3].u = TEXTURE_END;
	vertexs_[3].v = TEXTURE_END;

	//インデックス登録
	indexes_[0] = VERTEX_LEFT_BOTTOM;
	indexes_[1] = VERTEX_LEFT_TOP;
	indexes_[2] = VERTEX_RIGHT_TOP;

	indexes_[3] = VERTEX_RIGHT_TOP;
	indexes_[4] = VERTEX_RIGHT_BOTTOM;
	indexes_[5] = VERTEX_LEFT_BOTTOM;
}

void PixelShaderRenderer::MakeScreenSquereVertex()
{
	MakeSquereVertex(DEFAULT_POSITION, DEFAULT_POSITION, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);
}
