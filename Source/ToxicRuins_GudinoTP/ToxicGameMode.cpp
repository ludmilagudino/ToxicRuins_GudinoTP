// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicGameMode.h"
#include "ToxicGameState.h"
#include "ToxicPlayerState.h"
#include "ToxicRuins_GudinoTPCharacter.h"

AToxicGameMode::AToxicGameMode()
{
	// activar tick para descontar el tiempo en cada frame 
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = AToxicGameState::StaticClass();
	PlayerStateClass = AToxicPlayerState::StaticClass();

	// reglas iniciales
	TiempoTotal = 300.0f; // 5 minutos
	PuntosParaGanar = 20;
}

void AToxicGameMode::BeginPlay()
{
	Super::BeginPlay();

	IniciarPartida();
}

void AToxicGameMode::IniciarPartida()
{
	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (GS)
	{
		GS->TiempoRestante = TiempoTotal;
		GS->bPartidaEnCurso = true;
		GS->PuntosParaGanar = PuntosParaGanar;
	}
}

void AToxicGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (GS && GS->bPartidaEnCurso)
	{
		GS->TiempoRestante -= DeltaTime;

		//verificar si alguien ay gano
		VerificarCondicionVictoria();

		if (GS->TiempoRestante <= 0.0f)
		{
			TerminarPartida();
		}
	}
}

void AToxicGameMode::TerminarPartida()
{
	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (GS)
	{
		GS->bPartidaEnCurso = false;

		// Busca jugador con mas puntos
		APlayerState* Ganador = nullptr;
		int32 MaxPuntos = -1;

		for (APlayerState* PS : GS->PlayerArray)
		{
			AToxicPlayerState* ToxicPS = Cast<AToxicPlayerState>(PS);
			if (ToxicPS && ToxicPS->Puntos > MaxPuntos)
			{
				MaxPuntos = ToxicPS->Puntos;
				Ganador = PS;
			}
		}

		// ganador
		FString NombreGanador = Ganador ? Ganador->GetPlayerName() : TEXT("Nadie");
		FString Mensaje = NombreGanador + TEXT(" gana con ") + FString::FromInt(MaxPuntos) + TEXT(" puntos!");

		// mensaje a todos los jugadores
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC)
			{
				AToxicRuins_GudinoTPCharacter* Character = Cast<AToxicRuins_GudinoTPCharacter>(PC->GetPawn());
				if (Character)
				{
					Character->Client_MostrarMensaje(Mensaje, FLinearColor::Yellow);
				}

				//deshabilitar input del jugador
				//PC->DisableInput(PC);
			}
		}
	}
}

void AToxicGameMode::VerificarCondicionVictoria()
{
	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (!GS || !GS->bPartidaEnCurso)
	{
		return;
	}

	// verificar si alguien llego con los puntos para ganar
	for (APlayerState* PS : GS->PlayerArray)
	{
		AToxicPlayerState* ToxicPS = Cast<AToxicPlayerState>(PS);
		if (ToxicPS && ToxicPS->Puntos >= PuntosParaGanar)
		{
			// Guardar nombre del ganador antes de terminar
			GS->NombreGanador = PS->GetPlayerName();
			TerminarPartida();
			return;
		}
	}
}

void AToxicGameMode::VerificarJugadoresVivos()
{
	AToxicGameState* GS = GetGameState<AToxicGameState>();
	if (!GS || !GS->bPartidaEnCurso)
	{
		return;
	}

	int32 JugadoresVivos = 0;
	APlayerState* UltimoVivo = nullptr;

	for (APlayerState* PS : GS->PlayerArray)
	{
		AToxicPlayerState* ToxicPS = Cast<AToxicPlayerState>(PS);
		if (ToxicPS && ToxicPS->bEstaVivo)
		{
			JugadoresVivos++;
			UltimoVivo = PS;
		}
	}

	// Si queda 1 o menos jugadores vivos, terminar partida
	if (JugadoresVivos <= 1)
	{
		FString NombreGanador = UltimoVivo ? UltimoVivo->GetPlayerName() : TEXT("Nadie");
		GS->NombreGanador = NombreGanador;
		NotificarFinPartida(NombreGanador + TEXT(" es el último sobreviviente!"));
		TerminarPartida();
	}


}

void AToxicGameMode::NotificarFinPartida(const FString& NombreGanador)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			AToxicRuins_GudinoTPCharacter* Character = Cast<AToxicRuins_GudinoTPCharacter>(PC->GetPawn());
			if (Character)
			{
				Character->Client_MostrarMensaje(NombreGanador, FLinearColor::Yellow);
			}
		}
	}
}