// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EndStatisticsGameModeBase.generated.h"

/** 마지막 승리모션 씬
 * 
 */
UCLASS()
class DUOSHOOTING_API AEndStatisticsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
};