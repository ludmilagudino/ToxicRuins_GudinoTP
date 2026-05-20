// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicGameMode.h"
#include "ToxicGameState.h"
#include "ToxicPlayerState.h"

AToxicGameMode::AToxicGameMode()
{
	// Tick
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = AToxicGameState::StaticClass();
	PlayerStateClass = AToxicPlayerState::StaticClass();

	// reglas iniciales
	TiempoTotal = 300.0f; // 5 minutos
	PuntosParaGanar = 100;
}

void AToxicGameMode::BeginPlay()
{
	Super::BeginPlay();

	IniciarPartida();
}

void AToxicGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (GS && GS->bPartidaEnCurso)
	{
		GS->TiempoRestante -= DeltaTime;

		if (GS->TiempoRestante <= 0.0f)
		{
			TerminarPartida();
		}
	}
}

void AToxicGameMode::IniciarPartida()
{
	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (GS)
	{
		GS->TiempoRestante = TiempoTotal;
		GS->bPartidaEnCurso = true;
	}
}

void AToxicGameMode::TerminarPartida()
{
	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (GS)
	{
		GS->bPartidaEnCurso = false;
		// determinar ganador
	}
}

void AToxicGameMode::VerificarCondicionVictoria()
{
	// verificar si alguien llego con los puntos para ganar
}