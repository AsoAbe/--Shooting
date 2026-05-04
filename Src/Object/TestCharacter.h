#pragma once
#include "CharacterBase.h"

class TestCharacter :
	public CharacterBase
{
public:
	TestCharacter(SceneGame* parent,const CharacterModelData& modelData);
	~TestCharacter(void) override;

private:
	void Update_Move() override;
	void SetParam() override;
};

