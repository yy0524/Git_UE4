// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Components/SHealthComponent.h"
#include "UnrealNetwork.h"
#include "SHepler.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;
	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentHealth = DefaultHealth;
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.
				AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
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

	SHelper::Debug(GetOwner()->GetName()+ " Health: "+ FString::SanitizeFloat(CurrentHealth));
}


//告诉复制规则必需写这个函数
void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHealthComponent, CurrentHealth);
}
