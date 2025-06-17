// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillSystemBaseUI.h"

#include "Components/HorizontalBox.h"
#include "UI/SkillCoolTimeUI.h"

void USkillSystemBaseUI::AddSkill(UTexture2D* texture, FText skillKeyName)
{
	//스킬 아이콘 ui를 생성
	USkillCoolTimeUI* coolTimeUI = CreateWidget<USkillCoolTimeUI>(this, OriginSkillCoolTimeUI);
	//스킬 정보를 ui에 담기
	coolTimeUI->SetSkillImage(texture);
	coolTimeUI->SetSkillKeyName(skillKeyName);
	//스킬 정보를 배열에 저장
	SkillCoolTimeUIList.Add(coolTimeUI);
	//스킬 정보를 박스에 차곡차곡 쌓기
	HorizontalBox_SkillList->AddChild(coolTimeUI);
	//생성 직후에는 size를 구하기 힘들다 함. 그래서 직접 넣어줌. (coolTimeUI->GetCachedGeometry().GetLocalSize().X)
	HorizontalBox_SkillList->SetRenderTranslation(FVector2D(HorizontalBox_SkillList->GetRenderTransform().Translation.X - 158, HorizontalBox_SkillList->GetRenderTransform().Translation.Y));
}

void USkillSystemBaseUI::SetSkillCoolTimeUI(int32 index, float upper, float lower)
{
	//넣은 순서에 맞추어(좌측으로 갈수록 Index 작아짐) 대상을 꺼낸 뒤, 프로그레스바 퍼센트 조절
	SkillCoolTimeUIList[index]->SetSkillCoolTimeUI(upper, lower);
}

void USkillSystemBaseUI::SetActiveSkillIcon(int32 index, bool bActive, bool bForbidden)
{
	SkillCoolTimeUIList[index]->SetActiveDesign(bActive, bForbidden);
}

void USkillSystemBaseUI::AddAdditionalIcon(int32 index, UUserWidget* widget)
{
	SkillCoolTimeUIList[index]->SetAdditionalUI(widget);
}

void USkillSystemBaseUI::SetSkillRemainCoolTimeUI(int32 index, int32 remainTime, bool bEmpty)
{
	SkillCoolTimeUIList[index]->SetCoolTimeText(remainTime, bEmpty);
}

// FVector2D USkillSystemBaseUI::GetSkillIconPosition(int32 index)
// {
// 	return SkillCoolTimeUIList[index]->GetCachedGeometry().GetAbsolutePosition();
// }
