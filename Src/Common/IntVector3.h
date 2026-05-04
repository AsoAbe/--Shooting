#pragma once

#include<DxLib.h>

struct IntVector3
{
public:
	int x, y, z;
	IntVector3() :x(0), y(0), z(0) {};
	IntVector3(int inX, int inY, int inZ) :x(inX), y(inY), z(inZ) {};
	IntVector3(const VECTOR& v) :x(static_cast<int>(v.x)), y(static_cast<int>(v.y)), z(static_cast<int>(v.z)) {};

	//float値のVECTORをint値のIntVector3にする
	static IntVector3 VECTORToIntVector3(const VECTOR& v)
	{
		return IntVector3(v);
	}
	//intからDxLib仕様(float)に
	VECTOR ToVECTOR()const
	{
		return { static_cast<float>(x),static_cast<float>(y) ,static_cast<float>(z) };
	}

	static bool Equals(const IntVector3& v1, const IntVector3& v2)
	{
		return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
	}
	static IntVector3 VAdd(const IntVector3& v1, const IntVector3& v2)
	{
		return { v1.x + v2.x , v1.y + v2.y ,v1.z + v2.z };
	}
	static IntVector3 VSub(const IntVector3& v1, const IntVector3& v2)
	{
		return { v1.x - v2.x , v1.y - v2.y ,v1.z - v2.z };
	}

	//比較(map型で使われる)
	bool operator<(const IntVector3& v)const
	{
		if (x != v.x)
		{
			return x < v.x;
		}
		if (y != v.y)
		{
			return y < v.y;
		}
		return z < v.z;
	}
};