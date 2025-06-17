// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Management/EnumContainer.h"
#include "Management/NetworkGameInstance.h"
#include "EndStatisticsHelperActor.generated.h"

UCLASS()
class DUOSHOOTING_API AEndStatisticsHelperActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_SavedPlayStats)
	FFinalPlayStats SavedPlayStats;
	
	void Initialize(FFinalPlayStats stats);
	
public:
	// Sets default values for this actor's properties
	AEndStatisticsHelperActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> BlueprintSombra;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> BlueprintTracer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UChildActorComponent* ChildActorComponent;

public:
	void Server_SetPlayStats(FFinalPlayStats stats);

	UFUNCTION()
	void OnRep_SavedPlayStats();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
