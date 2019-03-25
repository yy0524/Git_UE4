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
#include "UnrealNetwork.h"
#include "SHepler.h"

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


void AWeapon::OnRep_HitScanTrace()
{
	SHelper::Debug("T:"+ HitScanTrace.TraceTo.ToString());
	PlayFireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.PhysicalSurfaceType, HitScanTrace.TraceTo);
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

	if (Role < ROLE_Authority)//先判断是在客户端，则调用服务器端的fire，这样达到客户端服务器端同时调用
	{
		ServerFire();
	}
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
		EPhysicalSurface SurfaceType;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, EyeLocation, EndLocation, COLLISION_WEAPON, Params))
		{
			AActor* HitActor = OutHit.GetActor();
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());
			TracerEnd = OutHit.ImpactPoint;

			float ActualDamage = DamageValue;
			if (SurfaceType == SURFACETYPE_FleshVulnerable)
			{
				ActualDamage = DamageValue * 4;
			}
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, OutHit, MyOwner->GetInstigatorController(), this, DamageType);


			PlayImpactEffect(SurfaceType, TracerEnd);
		
		}
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::Red, true, 1, 0, 1);
		}

		PlayFireEffect(TracerEnd);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEnd;
			SHelper::Debug("O:" + TracerEnd.ToString());
			HitScanTrace.PhysicalSurfaceType = SurfaceType;
		}
		
		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void AWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{

	UParticleSystem* SelectParticle = nullptr;

	switch (SurfaceType)
	{
	case SURFACETYPE_FleshDefault:
	case SURFACETYPE_FleshVulnerable:
		SelectParticle = FleshImpactEffect;
		break;
	default:
		SelectParticle = DefaultImpactEffect;
		break;
	}

	if (SelectParticle)
	{
		FVector StartTrace = Mesh->GetSocketLocation(MuzzleSocketName);
		FVector ShotDir = ImpactPoint - StartTrace;
		ShotDir.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectParticle, ImpactPoint,ShotDir.Rotation());
	}
}

void AWeapon::ServerFire_Implementation()
{
	Fire();
}

bool AWeapon::ServerFire_Validate()
{
	return true;
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

//告诉复制规则必需写这个函数
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon, HitScanTrace,COND_SkipOwner);//忽略自身
}