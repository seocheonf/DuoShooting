// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShootingMainWidget.generated.h"

/** 내 화면에 보여질 UI
 * 
 */
UCLASS()
class DUOSHOOTING_API UShootingMainWidget : public UUserWidget
{
	GENERATED_BODY()

	float MaxHealth;
	
protected:
	// 블루프린트에서 바인드
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentBullet;

	UPROPERTY(BlueprintReadOnly)
	int32 MyScore;

	UPROPERTY(BlueprintReadOnly)
	int32 MyTeamScore;

	UPROPERTY(BlueprintReadOnly)
	int32 EnemyTeamScore;
	
	// 체력
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CurrentHealthText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MaxHealthText;

	// 총알
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CurrentBulletText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MaxBulletText;

public:
	// 초반에 해줄 것
	void InitMaxHealth(float hp);
	void InitMaxBullet(int32 bullets);

	// 지속적으로 해줄 것
	void SetCurrentHealth(float hp);
	void SetCurrentBullet(int32 bullets);

	void SetMyScore(int32 myScore);
	void SetMyTeamScore(int32 myTeamScore);
	void SetEnemyTeamScore(int32 enemyTeamScore);
};
