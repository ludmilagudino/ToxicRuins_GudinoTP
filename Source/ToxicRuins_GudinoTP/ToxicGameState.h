// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ToxicGameState.generated.h"

/**
 *
 */
UCLASS()
class TOXICRUINS_GUDINOTP_API AToxicGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// variables replicadas
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	float TiempoRestante;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bPartidaEnCurso;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	FString NombreGanador;

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};