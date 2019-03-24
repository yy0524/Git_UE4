// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class UDamageType;
class UCameraShake;

//创建结构体记录子弹射线起始和结束位置
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> PhysicalSurfaceType;//网络复制中不识别枚举，所以要用TEnumAsByte强转

	UPROPERTY()
		FVector_NetQuantizeNormal TraceTo;
};

UCLASS()
class CSHOOTERGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* TraceEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float DamageValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float TimeBetweenShots;

	FTimerHandle  TimerHandle_TimeBetweenShots;

	float LastFireTime;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	//HitScanTrace发生变化触发此函数
	UFUNCTION()
	void OnRep_HitScanTrace();

protected:

	void PlayFireEffect(FVector TracerEndPoint);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

	//WithValidation检验合法性
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	void PlayImpactEffect(EPhysicalSurface SurfaceType,FVector ImpactPoint);

public:	

	void StartFire();
	void EndFire();

};
