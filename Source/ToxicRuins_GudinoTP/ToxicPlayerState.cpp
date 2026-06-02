// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicPlayerState.h"
#include "Net/UnrealNetwork.h"

AToxicPlayerState::AToxicPlayerState()
{
	Resistencia = 100.0f;
}

void AToxicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AToxicPlayerState, Puntos);
	DOREPLIFETIME(AToxicPlayerState, Resistencia);
}

void AToxicPlayerState::OnRep_Resistencia()
{
	// logica visual cuando cambia la resistencia
}
