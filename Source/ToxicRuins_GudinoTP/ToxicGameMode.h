// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToxicGameMode.generated.h"

/**
 *
 */
UCLASS()
class TOXICRUINS_GUDINOTP_API AToxicGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AToxicGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Reglas del juego
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Rules")
	float TiempoTotal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Rules")
	int32 PuntosParaGanar;

	// Funciones
	void IniciarPartida();
	void TerminarPartida();
	void VerificarCondicionVictoria();

	void VerificarJugadoresVivos();
	void NotificarFinPartida(const FString& NombreGanador);
};