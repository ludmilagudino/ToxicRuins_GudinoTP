// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToxicZone.generated.h"

UCLASS()
class TOXICRUINS_GUDINOTP_API AToxicZone : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AToxicZone();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// mesh visible de la zona toxica
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	// trigger para detectar jugadores
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	// cuanta resistnecia baja por segundo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toxic Zone")
	float DanioPorSegundo = 10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// jugadores dentro de la zona
	UPROPERTY()
	TArray<AActor*> JugadoresEnZona;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
