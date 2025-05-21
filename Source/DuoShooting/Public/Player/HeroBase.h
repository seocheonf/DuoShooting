// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroBase.generated.h"

UENUM()
enum class EHeroState : uint8
{
	Idle = 0,
	Attack,
	Die,
	NoSkill,
	UnVisible,
	LastIndex
};

UCLASS()
class DUOSHOOTING_API AHeroBase : public ACharacter
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this character's properties
	AHeroBase();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input / InitSkillSystemInput 기능을 함께 수행합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//=====변수=====
private:
	//스킬 시스템 (영웅은 스킬 시스템의 세부 내용을 직접 알 필요가 없다고 판단)
	class USkillSystemComponent* SkillSystemComp;
protected:
	//영웅 상태 (bitmask)
	int32 CurrentHeroState;
public:
	//=====함수=====
private:
	//스킬 시스템에 본인과 본인의 입력을 등록하는 함수입니다.
	void InitSkillSystemInput(class UInputComponent* playerInputComponent);
	//영웅 상태 bitmask 계산 추가
	void AddCurrentHeroState(EHeroState newState);
	//영웅 상태 bitmask 계산 제거
	void RemoveCurrentHeroState(EHeroState oldState);
protected:
	//생성자 단에서 초기화할 함수입니다. Super를 실행해주세요.
	virtual void InitConstructor();
	//스킬 시스템을 각 캐릭터마다 설정해 주세요. 적용할 스킬 시스템을 반환시켜주면 됩니다.
	FORCEINLINE virtual class USkillSystemComponent* InitSkillSystemComponent() { return nullptr; };
	//일단은 참고만 할 수 있게 놔뒀습니다. 필요하면 상의 후 const를 지워주세요.
	USkillSystemComponent* GetSkillSystemComponent() const;
public:
	//영웅 상태 bitmask 결과. 영웅의 상태에 대한 정보가 필요할 때 사용합니다.
	TArray<EHeroState> GetCurrentHeroState();
};
