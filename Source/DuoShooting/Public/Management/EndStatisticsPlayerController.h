// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EndStatisticsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API AEndStatisticsPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEndStatisticsMain> EndStatisticsMainFactory;

	UPROPERTY()
	class UEndStatisticsMain* EndStatisticsMainWidget;
	
public:
	AEndStatisticsPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void FindSharedCamera();

	void CreateMainWidget();

	virtual void BeginPlay() override;
};
