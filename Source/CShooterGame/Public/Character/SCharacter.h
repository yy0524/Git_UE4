// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class USHealthComponent;

UCLASS()
class CSHOOTERGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void StartZoom();
	void EndZoom();

	void StartFire();
	void EndFire();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDatle, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
		USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
		UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom")
		float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom",meta = (ClampMin = 0.1,ClampMax = 100))
		float ZoomInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		USHealthComponent* HealthComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
		bool bDied;

	float DefaultFOV;

	bool bWantsToZoom;//ÊÇ·ñÏë¿ª¾µ

	AWeapon* CurrentWeapon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

};
