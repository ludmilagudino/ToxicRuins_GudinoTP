// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableResource.generated.h"

UCLASS()
class TOXICRUINS_GUDINOTP_API ACollectableResource : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACollectableResource();

	// mesh visible del recurso
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	// para no agarrarlo dos veces
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resource")
	bool bFueAgarrado;

	// funcion para ser agarrado por un jugador
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void SerAgarrado(AActor* Jugador);

protected:
	// Replicación
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};