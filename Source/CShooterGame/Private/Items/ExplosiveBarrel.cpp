// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Items/ExplosiveBarrel.h"
#include "Public/Components/SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInterface.h"


// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Cast<USceneComponent>(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionObjectType(ECC_PhysicsBody);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>("HealthComponent");
	HealthComponent->OnHealthChange.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);

	ExplosionRadialForce = CreateDefaultSubobject<URadialForceComponent>("ExplosionRadialForce");
	ExplosionRadialForce->SetupAttachment(Mesh);
	ExplosionRadialForce->Radius = 250.f;//���ð뾶
	ExplosionRadialForce->bImpulseVelChange = true;//���彫��������������������ǵ���һ���̶����ٶȱ仯
	ExplosionRadialForce->bAutoActivate = false;
	ExplosionRadialForce->bIgnoreOwningActor = true;

	ExplosionImpulse = 400.f;
	bExplosived = false;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MI(TEXT("/Game/ExplosiveBarrel/M_ExplosiveBarrelExploded"));
	ExplosionMaterial = MI.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("/Game/WeaponEffects/Explosion/P_Explosion"));
	ExplosionParticle = PS.Object;

}


void AExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDatle, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExplosived)
	{
		return;
	}
	if (Health <= 0.f)
	{
		bExplosived = true;
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		Mesh->AddImpulse(BoostIntensity, NAME_None, true);
		if (ExplosionParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation());
		}

		if (ExplosionMaterial)
		{
			Mesh->SetMaterial(0, ExplosionMaterial);
		}
		
		ExplosionRadialForce->FireImpulse();//����Χ����Ӱ��
	}
}


