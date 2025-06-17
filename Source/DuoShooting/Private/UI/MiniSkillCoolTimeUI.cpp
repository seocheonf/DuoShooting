// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MiniSkillCoolTimeUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMiniSkillCoolTimeUI::SetGage(float upper, float lower)
{
	ProgressBar_Gage->SetPercent(upper/lower);
}

void UMiniSkillCoolTimeUI::SetCount(int32 count)
{
	Text_Count->SetText(FText::FromString(FString::FromInt(count)));
}
