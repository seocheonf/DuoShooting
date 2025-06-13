// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumContainer.h"
#include "GameFramework/GameState.h"
#include "TeamFightGameState.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API ATeamFightGameState : public AGameState
{
	GENERATED_BODY()

	//==기본 상속==//
protected:
	virtual void BeginPlay() override;

	//==변수==//
private:

	//==함수==//
public:

};
