// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniSkillCoolTimeUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UMiniSkillCoolTimeUI : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar_Gage;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Count;

public:
	void SetGage(float upper, float lower);
	void SetCount(int32 count);
	
};
