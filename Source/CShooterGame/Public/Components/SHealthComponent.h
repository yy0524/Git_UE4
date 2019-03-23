// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, USHealthComponent*, HealthComponent, float, Health, float, HealthDatle, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSHOOTERGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
		float DefaultHealth;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
		float CurrentHealth;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangeSignature OnHealthChange;
		
};
