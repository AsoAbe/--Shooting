#pragma once
#include <DxLib.h>

/*** ColorƒNƒ‰ƒX ***/
class
{
public:
	int Black;
	int Red;
	int BRed;
	int Green;
	int Blue;
	int White;
	int SkyBlue;
	int Yellow;
	int Purple;
	int Pink;
	int LinePink;

	void Read()
	{
		White = GetColor(255, 255, 255);
		Red = GetColor(255, 0, 0);
		BRed = 0x541222;
		Green = GetColor(0, 255, 0);
		Blue = GetColor(0, 0, 255);
		Black = GetColor(0, 0, 0);
		SkyBlue = 0x2df8fc;
		Yellow = 0xffff00;
		Purple = 0x7400a1;
		Pink = 0xf19cbb;
		LinePink = 0xec3b83;
	}
private:

}Col;