// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class UMaterialInstanceDynamic;

UCLASS()
class CSHOOTERGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Componets")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Componets")
		USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float ExplosionRadius;

	
	FVector NextPathPoint;

	UMaterialInstanceDynamic* MatInst;

	bool bExplosived;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void HandleTakeDamage(USHealthComponent* HealthComp, float Health, float HealthDatle, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	void SelfDestruct();//Ïú»Ù×ÔÉí
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
