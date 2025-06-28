// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EHeroInfo : uint8
{
	Tracer,
	Sombra
};

UENUM(BlueprintType)
enum class ETeamInfo : uint8
{
	A,
	B
};

/**
 * 
 */
class DUOSHOOTING_API EnumContainer
{
public:
	EnumContainer();
	~EnumContainer();
};
