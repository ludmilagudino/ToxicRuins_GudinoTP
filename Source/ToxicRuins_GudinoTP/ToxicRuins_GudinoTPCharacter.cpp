// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToxicRuins_GudinoTPCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ToxicRuins_GudinoTP.h"
#include "CollectableResource.h"
#include "DeliveryPoint.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"
#include "ToxicGameMode.h"
#include "ToxicPlayerState.h"

AToxicRuins_GudinoTPCharacter::AToxicRuins_GudinoTPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Inicializar variables de pickup
	bTieneObjeto = false;
	ObjetoAgarrado = nullptr;

	//variable participantes vivos
	bEstaVivo = true;
}

void AToxicRuins_GudinoTPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicar bTieneObjeto a todos los clientes
	DOREPLIFETIME(AToxicRuins_GudinoTPCharacter, bTieneObjeto);

	DOREPLIFETIME(AToxicRuins_GudinoTPCharacter, bEstaVivo);
}

void AToxicRuins_GudinoTPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AToxicRuins_GudinoTPCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AToxicRuins_GudinoTPCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AToxicRuins_GudinoTPCharacter::Look);
	}
	else
	{
		UE_LOG(LogToxicRuins_GudinoTP, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AToxicRuins_GudinoTPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AToxicRuins_GudinoTPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AToxicRuins_GudinoTPCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AToxicRuins_GudinoTPCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AToxicRuins_GudinoTPCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AToxicRuins_GudinoTPCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AToxicRuins_GudinoTPCharacter::PickupObjeto()
{
	if (!HasAuthority())
	{
		Server_PickupObjeto();
		return;
	}

	// Si ya tengo un objeto agarrado, no agarrar otro
	if (bTieneObjeto)
	{
		Client_MostrarMensaje(TEXT("Ya tenés un objeto!"), FLinearColor::Red);
		return;
	}

	// Line Trace desde la cámara hacia adelante
	FVector StartLocation = GetFollowCamera()->GetComponentLocation();
	FVector ForwardVector = GetFollowCamera()->GetForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 500.0f); // 500cm de alcance

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Trace con grosor para facilitar pickup
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(100.0f),
		QueryParams
	);

	// Si golpeó algo
	if (bHit)
	{
		// Verificar si es un CollectableResource
		ACollectableResource* Recurso = Cast<ACollectableResource>(HitResult.GetActor());

		if (Recurso != nullptr)
		{
			// Llamar función del recurso
			Recurso->SerAgarrado(this);

			// Guardar referencia y marcar que tenemos objeto
			ObjetoAgarrado = Recurso;
			bTieneObjeto = true;

			Client_MostrarMensaje(TEXT("Recurso agarrado!"), FLinearColor::Green);
		}
	}
}

void AToxicRuins_GudinoTPCharacter::DropObjeto()
{
	if (!HasAuthority())
	{
		Server_DropObjeto();
		return;
	}

	// Verificar que tengamos un objeto agarrado
	if (!bTieneObjeto)
	{
		Client_MostrarMensaje(TEXT("No tenés ningún objeto para entregar!"), FLinearColor::Red);
		return;
	}

	// Buscar un DeliveryPoint cercano
	TArray<FOverlapResult> Overlaps;
	FVector PlayerLocation = GetActorLocation();
	float Radio = 300.0f; // 3 metros

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		PlayerLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radio)
	);

	// Buscar un DeliveryPoint en los overlaps
	ADeliveryPoint* PuntoEntrega = nullptr;
	for (FOverlapResult& Overlap : Overlaps)
	{
		PuntoEntrega = Cast<ADeliveryPoint>(Overlap.GetActor());
		if (PuntoEntrega != nullptr)
		{
			break;
		}
	}

	if (PuntoEntrega != nullptr)
	{
		// Entregar el recurso
		PuntoEntrega->EntregarRecurso(this);
	}
	else
	{
		// No hay punto de entrega cerca
		Client_MostrarMensaje(TEXT("No hay zona de entrega cerca!"), FLinearColor(1.0f, 0.5f, 0.0f, 1.0f));
	}
}

void AToxicRuins_GudinoTPCharacter::Server_PickupObjeto_Implementation()
{
	PickupObjeto();
}

void AToxicRuins_GudinoTPCharacter::Server_DropObjeto_Implementation()
{
	DropObjeto();
}

void AToxicRuins_GudinoTPCharacter::Client_MostrarMensaje_Implementation(const FString& Mensaje, FLinearColor Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color.ToFColor(true), Mensaje);
	}
}

void AToxicRuins_GudinoTPCharacter::Multicast_JugadorMurio_Implementation()
{
	// todos ven al personaje desaparecer
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// mensaje en pantalla
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Murió un jugador"));
	}

	// sonido
	BP_ReproducirSonidoMuerte();
}

void AToxicRuins_GudinoTPCharacter::MorirJugador()
{
	if (!HasAuthority())
	{
		return;
	}

	bEstaVivo = false;

	// Marcar en el PlayerState PRIMERO
	AToxicPlayerState* PS = GetPlayerState<AToxicPlayerState>();
	if (PS)
	{
		PS->bEstaVivo = false;
	}

	// deshabilitar input
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->DisableInput(PC);
	}

	// notificar a todos
	Multicast_JugadorMurio();

	// DESPUÉS verificar jugadores vivos
	AToxicGameMode* GM = Cast<AToxicGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->VerificarJugadoresVivos();
	}
}