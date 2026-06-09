// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectableResource.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACollectableResource::ACollectableResource()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// replicación
	bReplicates = true;

	// crear mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	//replico el mesh component asi cuando desaparezca visualmente el recurso en el servidor desaparezca tambien en los clientes
	MeshComponent->SetIsReplicated(true);

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

	// bool agarrado
	bFueAgarrado = true;

	// Debug
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("SerAgarrado ejecutado"));
	}

	Destroy();
}