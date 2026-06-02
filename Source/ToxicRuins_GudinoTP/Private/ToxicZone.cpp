// Fill out your copyright notice in the Description page of Project Settings.


#include "ToxicZone.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ToxicRuins_GudinoTPCharacter.h"
#include "ToxicPlayerState.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AToxicZone::AToxicZone()
{
    PrimaryActorTick.bCanEverTick = true;

    // Crear componente raíz vacío
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // Mesh se adjunta al root
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    // Trigger se adjunta al root
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f));

    // Solo overlap, no bloquea
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void AToxicZone::BeginPlay()
{
    Super::BeginPlay();

    // Conectar eventos del trigger
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AToxicZone::OnTriggerBeginOverlap);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AToxicZone::OnTriggerEndOverlap);
}


//cuando el jugador entra
void AToxicZone::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // ignorar si no capsula del jugador
    if (!OtherComp->IsA<UCapsuleComponent>())
    {
        return;
    }

    AToxicRuins_GudinoTPCharacter* Jugador = Cast<AToxicRuins_GudinoTPCharacter>(OtherActor);
    if (Jugador != nullptr && !JugadoresEnZona.Contains(OtherActor))
    {
        JugadoresEnZona.Add(OtherActor);

        // Debug
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Jugador entró a zona tóxica!"));
        }
    }
}

//cuando el jugador sale
void AToxicZone::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // ignorar si no capsula del jugador
    if (!OtherComp->IsA<UCapsuleComponent>())
    {
        return;
    }

    JugadoresEnZona.Remove(OtherActor);

    // Debug
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Jugador salió de zona tóxica!"));
    }
}

void AToxicZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // solo el servidor procesa el daño
    if (!HasAuthority())
    {
        return;
    }

    // bajar resistencia a todos los jugadores en la zona
    for (AActor* Actor : JugadoresEnZona)
    {
        AToxicRuins_GudinoTPCharacter* Jugador = Cast<AToxicRuins_GudinoTPCharacter>(Actor);
        if (Jugador && Jugador->bEstaVivo)
        {
            AToxicPlayerState* PS = Jugador->GetPlayerState<AToxicPlayerState>();
            if (PS)
            {
                PS->Resistencia -= DanioPorSegundo * DeltaTime;

                // Debug resistencia
                if (GEngine)
                {
                    FString Msg = FString::Printf(TEXT("Resistencia: %.1f"), PS->Resistencia);
                    GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, Msg);
                }

                // si la resistencia llega a 0, el jugador muere
                if (PS->Resistencia <= 0.0f)
                {
                    PS->Resistencia = 0.0f;

                    // Debug muerte
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Jugador murió por resistencia!"));
                    }

                    Jugador->MorirJugador();
                }
            }
        }
    }
}