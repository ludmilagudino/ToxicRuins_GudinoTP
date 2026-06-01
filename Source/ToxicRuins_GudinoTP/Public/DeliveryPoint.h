// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryPoint.generated.h"

UCLASS()
class TOXICRUINS_GUDINOTP_API ADeliveryPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeliveryPoint();

	// plataforma
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PlatformMesh;

	// detectar jugadores
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	// puntos  al entregar un recurso
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	int32 PuntosRecompensa = 10;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//  actor entra al trigger
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// actor sale del trigger
	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	

	// entregar recurso (Character  presiona E)
	UFUNCTION(BlueprintCallable, Category = "Delivery")
	void EntregarRecurso(AActor* Jugador);
};
