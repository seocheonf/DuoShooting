// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EndStatisticsHelperActor.h"

#include "Net/UnrealNetwork.h"
#include "Player/RepChildActorComponent.h"
#include "Player/SombraHero.h"
#include "UI/EndStatisticsIndividual.h"
#include "UI/RepWidgetComponent.h"

// Sets default values
AEndStatisticsHelperActor::AEndStatisticsHelperActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;

	{
		ConstructorHelpers::FClassFinder<ACharacter> tempCharacter(TEXT("'/Game/DuoShooting/Blueprints/Characters/Sombra/BP_LobbySombra.BP_LobbySombra_C'"));
		if (tempCharacter.Succeeded()) BlueprintSombra = tempCharacter.Class;		
	}

	{
		ConstructorHelpers::FClassFinder<ACharacter> tempCharacter(TEXT("'/Game/DuoShooting/Blueprints/Characters/BP_LobbyTracer.BP_LobbyTracer_C'"));
		if (tempCharacter.Succeeded()) BlueprintTracer = tempCharacter.Class;	
	}
	
	// 캐릭터를 넣을 컴포넌트의 껍데기만 만들어 두자
	ChildActorComponent = CreateDefaultSubobject<URepChildActorComponent>(TEXT("ChildActorComponent"));
	SetRootComponent(ChildActorComponent);
	//ChildActorComponent->SetIsReplicated(true);
	ChildActorComponent->SetVisibility(true);
	
	// 위젯 컴포넌트를 생성자에서 만들어 놓기만 하자
	WidgetComponent = CreateDefaultSubobject<URepWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ConstructorHelpers::FClassFinder<UEndStatisticsIndividual> TempWidget(
		TEXT("'/Game/DuoShooting/UIs/WBP_EndStatisticsIndividual.WBP_EndStatisticsIndividual_C'"));
	if (TempWidget.Succeeded()) { WidgetComponent->SetWidgetClass(TempWidget.Class); }
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetRelativeLocation(FVector(40.0f, 50.0f, 20.0f));
	WidgetComponent->SetRelativeScale3D(FVector(1.0f, 0.3f, 0.3f));
	//WidgetComponent->SetIsReplicated(true);
	WidgetComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AEndStatisticsHelperActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[%s]AEndStatisticsHelperActor BeginPlay"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
}

// Called every frame
void AEndStatisticsHelperActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 서버만
void AEndStatisticsHelperActor::Server_SetPlayStats(FFinalPlayStats stats)
{
	SavedPlayStats = stats;
	Initialize(SavedPlayStats);
}

void AEndStatisticsHelperActor::OnRep_SavedPlayStats()
{
	Initialize(SavedPlayStats);
}

void AEndStatisticsHelperActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEndStatisticsHelperActor, SavedPlayStats);
}

void AEndStatisticsHelperActor::Initialize(FFinalPlayStats stats)
{
	if (ChildActorComponent == nullptr) return;
	
	switch (stats.HeroInfo)
	{
	case EHeroInfo::Sombra:
		if (BlueprintSombra) ChildActorComponent->SetChildActorClass(BlueprintSombra);
		break;
	case EHeroInfo::Tracer:
		if (BlueprintTracer) ChildActorComponent->SetChildActorClass(BlueprintTracer);
		break;
	}

	if (WidgetComponent)
	{
		WidgetComponent->InitWidget();

		if (auto individualWidget = Cast<UEndStatisticsIndividual>(WidgetComponent->GetWidget()))
		{
			individualWidget->SetUserName(stats.UserName);
			individualWidget->SetScore(stats.Score);
		}
	}
}
