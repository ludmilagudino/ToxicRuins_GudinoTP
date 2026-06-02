// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ToxicRuins_GudinoTPCharacter.h"
#include "ToxicPlayerState.h"
#include "CollectableResource.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ADeliveryPoint::ADeliveryPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Crear componente raíz vacío
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Mesh se adjunta al root
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(RootComponent);

	// Trigger se adjunta al root
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f));

	// Solo overlap, no bloquea
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// valores
	PuntosRecompensa = 10;
}

// Called when the game starts or when spawned
void ADeliveryPoint::BeginPlay()
{
	Super::BeginPlay();

	// eventos del trigger
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryPoint::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADeliveryPoint::OnTriggerEndOverlap);
}

void ADeliveryPoint::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp->IsA<UCapsuleComponent>())
	{
		return;
	}

	AToxicRuins_GudinoTPCharacter* Jugador = Cast<AToxicRuins_GudinoTPCharacter>(OtherActor);
	if (Jugador != nullptr)
	{
		// AGREGAR a la lista
		if (!JugadoresEnZona.Contains(OtherActor))
		{
			JugadoresEnZona.Add(OtherActor);
		}
		Jugador->Client_MostrarMensaje(TEXT("Entraste a la zona de entrega!"), FLinearColor::Blue);
	}
}

void ADeliveryPoint::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherComp->IsA<UCapsuleComponent>())
	{
		return;
	}

	AToxicRuins_GudinoTPCharacter* Jugador = Cast<AToxicRuins_GudinoTPCharacter>(OtherActor);
	if (Jugador != nullptr)
	{
		// SACAR de la lista
		JugadoresEnZona.Remove(OtherActor);
		Jugador->Client_MostrarMensaje(TEXT("Saliste de la zona de entrega!"), FLinearColor::White);
	}
}
void ADeliveryPoint::EntregarRecurso(AActor* Jugador)
{
	
	if (!HasAuthority())
	{
		return;
	}


	AToxicRuins_GudinoTPCharacter* Character = Cast<AToxicRuins_GudinoTPCharacter>(Jugador);
	if (Character == nullptr)
	{
		return;
	}

	// verificar que tenga un objeto agarrado
	if (Character->ObjetoAgarrado == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No tenés ningún objeto para entregar!"));
		}
		return;
	}

	// destruir el objeto agarrado
	if (Character->ObjetoAgarrado != nullptr)
	{
		Character->ObjetoAgarrado->Destroy();
		Character->ObjetoAgarrado = nullptr;
	}

	// limpiar el bool
	Character->bTieneObjeto = false;

	// sumar puntos al PlayerState
	AToxicPlayerState* PlayerState = Character->GetPlayerState<AToxicPlayerState>();
	if (PlayerState != nullptr)
	{
		PlayerState->Puntos += PuntosRecompensa;

		// Mostrar mensaje solo al jugador que entregó
		FString Mensaje = FString::Printf(TEXT("Recurso entregado! +%d puntos. Total: %d"),
			PuntosRecompensa, PlayerState->Puntos);
		Character->Client_MostrarMensaje(Mensaje, FLinearColor::Green);
	}
}

void ADeliveryPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		return;
	}

	for (AActor* Actor : JugadoresEnZona)
	{
		AToxicRuins_GudinoTPCharacter* Jugador = Cast<AToxicRuins_GudinoTPCharacter>(Actor);
		if (Jugador && Jugador->bEstaVivo)
		{
			AToxicPlayerState* PS = Jugador->GetPlayerState<AToxicPlayerState>();
			if (PS && PS->Resistencia < 100.0f)
			{
				PS->Resistencia += 10.0f * DeltaTime;

				if (PS->Resistencia > 100.0f)
				{
					PS->Resistencia = 100.0f;
				}
			}
		}
	}
}