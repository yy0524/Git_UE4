// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"
#include "SHepler.h"
#include "SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCanEverAffectNavigation(false);//不影响导航网格
	Mesh->SetSimulatePhysics(true);
	MovementForce = 1000.f;
	RequiredDistanceToTarget = 100.f;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChange.AddDynamic(this,&ASTrackerBot::HandleTakeDamage);
	bExplosived = false;
	ExplosionRadius = 200.f;
	ExplosionDamage = 40.f;
	
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	GetNextPathPoint();
	
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* HealthComp, float Health, float HealthDatle, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!MatInst)
	{
		MatInst = Mesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, Mesh->GetMaterial(0));	
	}
	else
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken",GetWorld()->TimeSeconds);
	}

	if (Health <= 0.f)
	{
		SelfDestruct();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this,0);

	UNavigationPath* NavPath =  UNavigationSystemV1::FindPathToActorSynchronously(this,GetActorLocation(),PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExplosived)
	{
		return;
	}
	bExplosived = true;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoreActors, this, GetInstigatorController(), true);
	
	DrawDebugSphere(GetWorld(),GetActorLocation(),ExplosionRadius,12,FColor::Red,false,2.f,0,1.0f);

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplosionEffect,GetActorLocation());
	}
	
	Destroy();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;
		Mesh->AddForce(ForceDirection,NAME_None,true);
	}

}

// Called to bind functionality to input
void ASTrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

