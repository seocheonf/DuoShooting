// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillCoolTimeUI.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void USkillCoolTimeUI::SetSkillImage(UTexture2D* texture)
{
	if (texture == nullptr)
		return;

	if (Image_SkillIcon == nullptr)
		return;
	
	Image_SkillIcon->SetBrushFromTexture(texture, true);
	
	Image_SkillIcon->InvalidateLayoutAndVolatility();
}

void USkillCoolTimeUI::SetSkillCoolTimeUI(float upper, float lower)
{
	ProgressBar_CoolTime->SetPercent(upper/lower);
}

void USkillCoolTimeUI::SetSkillKeyName(FText skillKeyName)
{
	Text_Key->SetText(skillKeyName);
}

void USkillCoolTimeUI::SetActiveDesign(bool bActive, bool bForbidden)
{
	if (bActive)
	{
		//활성화 되면 게이지 초기화 하고, 아이콘을 밝게 한다.
		ProgressBar_CoolTime->SetPercent(0.f);
		Image_Activation->SetColorAndOpacity(FLinearColor(0,0,0,0));
	}
	else
	{
		//비활성화 되면 게이지 초기화 하고, 아이콘을 어둡게 한다.
		ProgressBar_CoolTime->SetPercent(0.f);
		Image_Activation->SetColorAndOpacity(FLinearColor(0,0,0,0.25f));
		//이 때 금지 상태라는 효과가 온다면, 더 강렬하게 금지한다
		if (bForbidden)
		{
			Image_Activation->SetColorAndOpacity(FLinearColor(1,0,0,0.25f));
		}
	}
}

void USkillCoolTimeUI::SetAdditionalUI(UUserWidget* widget)
{
	UVerticalBoxSlot* slot = VerticalBox_Additional->AddChildToVerticalBox(widget);
	slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
}

void USkillCoolTimeUI::SetCoolTimeText(int32 remainTime, bool bEmpty)
{
	if (bEmpty)
	{
		Text_CoolTime->SetText(FText::GetEmpty());
		return;
	}

	if (Text_CoolTime != nullptr)
		Text_CoolTime->SetText(FText::FromString(FString::FromInt(remainTime + 1)));

}


