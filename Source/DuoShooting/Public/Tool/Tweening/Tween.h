// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

DECLARE_DELEGATE(FTweenCompleteDelegate)

UENUM(BlueprintType)
enum class EEasing : uint8
{
	Linear,
};

class DUOSHOOTING_API Tween
{
public:
	Tween();
	~Tween();
};
