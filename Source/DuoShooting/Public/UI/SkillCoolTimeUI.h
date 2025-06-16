// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillCoolTimeUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API USkillCoolTimeUI : public UUserWidget
{
	GENERATED_BODY()

	//==변수==
private:

	//스킬 키
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Key;
	//스킬 쿨타임 프로그레스바
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar_CoolTime;
	//스킬 아이콘
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_SkillIcon;
	//스킬 활성화 이미지
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Activation;
	
public:

	//==함수==
private:
public:
	//프로그래스바 뒷 배경 UI를 설정하는 함수
	void SetSkillImage(UTexture2D* texture);

	//프로그래스바 퍼센트 조절 (쿨타임 디자인 퍼센트 조절)
	void SetSkillCoolTimeUI(float upper, float lower);

	//스킬 키 이름 설정
	void SetSkillKeyName(FText skillKeyName);

	//스킬 사용 가능 여부 색 조정
	void SetActiveDesign(bool bActive);
};
