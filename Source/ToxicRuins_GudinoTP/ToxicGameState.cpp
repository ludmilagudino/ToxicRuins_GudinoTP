// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicGameState.h"
#include "Net/UnrealNetwork.h"


void AToxicGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToxicGameState, TiempoRestante);
	DOREPLIFETIME(AToxicGameState, bPartidaEnCurso);
	DOREPLIFETIME(AToxicGameState, NombreGanador);
	DOREPLIFETIME(AToxicGameState, PuntosParaGanar);
}