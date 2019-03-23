// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

/**
 * 
 */
namespace SHelper//ÆÁÄ»ÉÏÊä³ö
{
	FORCEINLINE void Debug(FString Message, float Duration = 3.f)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Yellow, Message);
		}
	}
}
