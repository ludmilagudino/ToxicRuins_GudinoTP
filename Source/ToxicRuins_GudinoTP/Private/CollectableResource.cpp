// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectableResource.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACollectableResource::ACollectableResource()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Habilitar replicación
	bReplicates = true;

	// Crear mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetIsReplicated(true);

	// Inicializar variables
	bFueAgarrado = false;
}

void ACollectableResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableResource, bFueAgarrado);
}

void ACollectableResource::SerAgarrado(AActor* Jugador)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bFueAgarrado)
	{
		return;
	}

	// agarrado
	bFueAgarrado = true;

	// Debug
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("SerAgarrado ejecutado"));
	}

	Destroy();
}