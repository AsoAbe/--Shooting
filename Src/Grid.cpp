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
	VECTOR sPos = { -HLEN,GRID_ORIGIN ,GRID_ORIGIN };//始点
	VECTOR ePos = { HLEN,GRID_ORIGIN ,GRID_ORIGIN };//終点
	for (int i = HNUM * GRID_LINE_START; i <= HNUM; i++)
	{
		sPos.z = i * TERM;
		ePos.z = i * TERM;
		DrawLine3D(sPos, ePos, GRID_X_AXIS_COLOR);
		DrawSphere3D(ePos, GRID_SPHERE_RADIUS, GRID_SPHERE_SEGMENTS, GRID_X_AXIS_COLOR, GRID_SPHERE_COLOR, true);
	}
	sPos = { GRID_ORIGIN ,GRID_ORIGIN ,-HLEN };//始点
	ePos = { GRID_ORIGIN ,GRID_ORIGIN ,HLEN };//終点
	for (int i = HNUM * GRID_LINE_START; i <= HNUM; i++)
	{
		sPos.x = i * TERM;
		ePos.x = i * TERM;
		DrawLine3D(sPos, ePos, GRID_Z_AXIS_COLOR);
		DrawSphere3D(ePos, GRID_SPHERE_RADIUS, GRID_SPHERE_SEGMENTS, GRID_Z_AXIS_COLOR, GRID_SPHERE_COLOR, true);
	}
	sPos = { GRID_ORIGIN ,-HLEN ,GRID_ORIGIN };//始点
	ePos = { GRID_ORIGIN ,HLEN  ,GRID_ORIGIN};//終点
	DrawLine3D(sPos, ePos, GRID_Y_AXIS_COLOR);
	DrawSphere3D(ePos, GRID_SPHERE_RADIUS, GRID_SPHERE_SEGMENTS, GRID_Y_AXIS_COLOR, GRID_SPHERE_COLOR, true);
#endif
}

void Grid::Release(void)
{
}
