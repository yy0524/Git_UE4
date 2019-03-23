// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Components/SHealthComponent.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = DefaultHealth;
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.
			AddDynamic(this,&USHealthComponent::HandleTakeAnyDamage);
	}
	
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage,0.f,DefaultHealth);
	OnHealthChange.Broadcast(this,CurrentHealth,Damage,DamageType,InstigatedBy,DamageCauser);
}



