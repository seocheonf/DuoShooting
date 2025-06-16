// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSystemBaseUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API USkillSystemBaseUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	//변수
private:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalBox_SkillList;
	//스킬 쿨타임 리스트
	TArray<class USkillCoolTimeUI*> SkillCoolTimeUIList;
	//스킬 쿨타임 아이콘
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class USkillCoolTimeUI> OriginSkillCoolTimeUI;
	//현재 스킬 개수
	int32 SkillCount = 0;
	//함수
public:
	//스킬 정보 추가
	void AddSkill(UTexture2D* texture, FText skillKeyName);
	
	//스킬 쿨타임 조정 함수
	void SetSkillCoolTimeUI(int32 index, float upper, float lower);

	//스킬 활성화 색 조정 함수
	void SetActiveSkillIcon(int32 index, bool bActive);
};
