#include <DxLib.h>
#include "Grid.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

Grid::Grid(void)
{
}

Grid::~Grid(void)
{
}

void Grid::Init(void)
{
}

void Grid::Update(void)
{
}

void Grid::Draw(void)
{
#ifdef _DEBUG
	VECTOR sPos = { -HLEN,0 ,0 };//始点
	VECTOR ePos = { HLEN,0 ,0 };//終点
	for (int i = HNUM * -1; i <= HNUM; i++)
	{
		sPos.z = i * TERM;
		ePos.z = i * TERM;
		DrawLine3D(sPos, ePos, 0xFF0000);
		DrawSphere3D(ePos, 8, 8, 0xFF0000, 0x000000, true);
	}
	sPos = { 0 ,0 ,-HLEN };//始点
	ePos = { 0 ,0 ,HLEN };//終点
	for (int i = HNUM * -1; i <= HNUM; i++)
	{
		sPos.x = i * TERM;
		ePos.x = i * TERM;
		DrawLine3D(sPos, ePos, 0x0000FF);
		DrawSphere3D(ePos, 8, 8, 0x0000FF, 0x000000, true);
	}
	sPos = { 0 ,-HLEN ,0 };//始点
	ePos = { 0 ,HLEN  ,0};//終点
	DrawLine3D(sPos, ePos, 0x00FF00);
	DrawSphere3D(ePos, 8, 8, 0x00FF00, 0x000000, true);
#endif
}

void Grid::Release(void)
{
}
