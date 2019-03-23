// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CShooterGame.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("Shoot.DebugWeapons"),//命令行的名字
	DebugWeaponDrawing,//命令行的值
	TEXT("Draw Debug Lines for Weapons"),//对命令行的说明
	ECVF_Cheat //这参数表示游戏发布时，不会显示
);

// Sets default values
AWeapon::AWeapon()
{

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MuzzleSocketName = FName("MuzzleFlashSocket");
	DamageValue = 20.f;
	TimeBetweenShots = 0.1f;

	SetReplicates(true);//设置为可复制，这样客户端就有枪了

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ME(TEXT("/Game/WeaponEffects/Muzzle/P_Muzzle_Large.P_Muzzle_Large"));
	MuzzleEffect = ME.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> IE(TEXT("/Game/WeaponEffects/BloodImpact/P_blood_splash_02.P_blood_splash_02"));
	FleshImpactEffect = IE.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DE(TEXT("/Game/WeaponEffects/GenericImpact/P_RifleImpact.P_RifleImpact"));
	DefaultImpactEffect = DE.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> TE(TEXT("/Game/WeaponEffects/BasicTracer/P_SmokeTrail.P_SmokeTrail"));
	TraceEffect = TE.Object;

}


void AWeapon::PlayFireEffect(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, Mesh, MuzzleSocketName);
	}

	if (TraceEffect)
	{
		FVector MuzzleLocation = Mesh->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerTrail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
		if (TracerTrail)
		{
			TracerTrail->SetVectorParameter("BeamEnd", TracerEndPoint);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC && FireCameraShake)
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	
	}
}

void AWeapon::Fire()
{

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		FVector EndLocation = EyeLocation + ShotDirection * 10000;
		FVector TracerEnd = EndLocation;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(MyOwner);//忽略持有者
		Params.AddIgnoredActor(this);//忽略自己
		Params.bTraceComplex = true;
		Params.bReturnPhysicalMaterial = true;//设定这个参数才可以访问物理材质
		//FCollisionResponseParams ResponseParams;
	
		FHitResult OutHit;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, EyeLocation, EndLocation, COLLISION_WEAPON, Params))
		{
			AActor* HitActor = OutHit.GetActor();

			EPhysicalSurface PhysicalSurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());

			UParticleSystem* SelectParticle = nullptr;

			float DamageInstance;
			switch (PhysicalSurfaceType)
			{
			case SURFACETYPE_FleshDefault:
				SelectParticle = FleshImpactEffect;
				DamageInstance = DamageValue;
			case SURFACETYPE_FleshVulnerable:
				SelectParticle = FleshImpactEffect;
				DamageInstance = DamageValue * 4;
				break;
			default:
				SelectParticle = DefaultImpactEffect;
				DamageInstance = DamageValue * 4;
				break;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, DamageInstance, ShotDirection, OutHit, MyOwner->GetInstigatorController(), this, DamageType);

			if (SelectParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectParticle, OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation());
			}

			TracerEnd = OutHit.ImpactPoint;
		
		}
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::Red, true, 1, 0, 1);
		}

		PlayFireEffect(TracerEnd);
		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void AWeapon::StartFire()
{
	float InFirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots,this,&AWeapon::Fire,TimeBetweenShots,true,InFirstDelay);
}

void AWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

