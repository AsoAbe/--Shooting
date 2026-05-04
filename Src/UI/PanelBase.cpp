#include "PanelBase.h"

PanelBase::PanelBase(SceneGame& sceneGame):sceneGame_(sceneGame), isActive_(false)
{
}

PanelBase::~PanelBase()
{
}

void PanelBase::Update()
{
}

void PanelBase::Draw()
{
}

bool PanelBase::IsActive() const
{
    return isActive_;
}

void PanelBase::SetActive(bool b)
{
    isActive_ = b;
}
