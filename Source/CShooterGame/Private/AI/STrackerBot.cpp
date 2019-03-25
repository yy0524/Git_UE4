// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"

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
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	GetNextPathPoint();
	
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

