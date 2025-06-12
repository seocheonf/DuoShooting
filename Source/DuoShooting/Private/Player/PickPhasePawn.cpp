// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PickPhasePawn.h"

#include "Management/TeamFightGameMode.h"
#include "Management/TeamFightGameState.h"


// Sets default values
APickPhasePawn::APickPhasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FClassFinder<UPickPhaseUI> ui(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/DuoShooting/UIs/WBP_PickPhaseUI.WBP_PickPhaseUI'"));
	if (ui.Succeeded())
	{
		OriginPickPhaseUI = ui.Class;
	}
}

// Called when the game starts or when spawned
void APickPhasePawn::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&]()
	{
		if (Cast<APlayerController>(Controller) != nullptr && IsLocallyControlled())
		{
			ServerRPC_SetPlayerHero(EHeroInfo::Sombra);
			
		}
	}, 1, false);
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, [&]()
	{
		if (Cast<APlayerController>(Controller) != nullptr && IsLocallyControlled())
			ServerRPC_RespawnPlayer();
	}, 2, false);
	
}

// Called every frame
void APickPhasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APickPhasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APickPhasePawn::ServerRPC_SetPlayerHero_Implementation(EHeroInfo playerHero)
{
	//현재 월드의 게임 모드를 가져온다.
	ATeamFightGameMode* gameMode = Cast<ATeamFightGameMode>(GetWorld()->GetAuthGameMode());

	//만약 게임 모드가 없다면, return한다.
	//이 함수가 server rpc implementation이라 서버에서 돌아감이 보장되므로 nullptr이라면
	//서버에 게임 모드가 없는 비 정상적 상황인 것이기에 에러 메시지를 띄운다.
	if (nullptr == gameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode does not exist in Server!!!!!!"))
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(Controller);
	//받은 정보를 바탕으로 플레이어의 영웅 정보를 갱신시킨다.
	if (nullptr != playerController)
		gameMode->SetPlayerHero(playerController, playerHero);
}

void APickPhasePawn::ServerRPC_RespawnPlayer_Implementation()
{
	//현재 월드의 게임 모드를 가져온다.
	ATeamFightGameMode* gameMode = Cast<ATeamFightGameMode>(GetWorld()->GetAuthGameMode());

	//만약 게임 모드가 없다면, return한다.
	//이 함수가 server rpc implementation이라 서버에서 돌아감이 보장되므로 nullptr이라면
	//서버에 게임 모드가 없는 비 정상적 상황인 것이기에 에러 메시지를 띄운다.
	if (nullptr == gameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode does not exist in Server!!!!!!"))
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(Controller);
	//받은 정보를 바탕으로 플레이어의 영웅 정보를 갱신시킨다.
	if (nullptr != playerController)
		gameMode->RespawnPlayer(playerController);
}
