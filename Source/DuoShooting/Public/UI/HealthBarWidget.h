// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

	float MaxHealth;

protected:
	// 블루프린트에서 바인드
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHealth;
	
	// 체력
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

public:
	// 초반에 해줄 것
	void InitMaxHealth(float hp);

	// 지속적으로 해줄 것
	void SetCurrentHealth(float hp);

	// 같은팀용 UI 모드로 설정
	void ApplyMyTeamMode();

	// 적팀용 UI 모드로 설정
	void ApplyEnemyTeamMode();
};
