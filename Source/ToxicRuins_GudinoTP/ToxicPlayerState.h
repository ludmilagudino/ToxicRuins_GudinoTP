// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ToxicPlayerState.generated.h"

/**
 *
 */
UCLASS()
class TOXICRUINS_GUDINOTP_API AToxicPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AToxicPlayerState();

	// Variables replicadas
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State")
	int32 Puntos;

	UPROPERTY(ReplicatedUsing = OnRep_Resistencia, BlueprintReadOnly, Category = "Player State")
	float Resistencia;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State")
	int32 Equipo;

	// RepNotify para Resistencia
	UFUNCTION()
	void OnRep_Resistencia();

	// Función para configurar replicación
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};