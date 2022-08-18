// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteraction_Comp.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UDoorInteraction_Comp::UDoorInteraction_Comp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UDoorInteraction_Comp::TriggerBoxEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((DoorState == EDoorState::CLOSED) && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

		if (PlayerPawn)
		{
			if (FrameMesh)
			{
				if (FVector::DotProduct(FrameMesh->GetActorRightVector(), PlayerPawn->GetActorForwardVector()) >= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::TriggerBoxEntered - Player is facing door forward"));
					FinalRotation *= -1;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::TriggerBoxEntered - Player is not facing doorforward"));
				}
			}
			
			DoorState = EDoorState::OPENING;
		}
	}
}

void UDoorInteraction_Comp::KeyTriggerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

		if (PlayerPawn && KeyTrigger)
		{
			bIsLocked = false;
			KeyTrigger->Destroy();

			if (KeyMesh)
			{
				KeyMesh->Destroy();
			}
		}
	}
}

void UDoorInteraction_Comp::OpeningDoor(float DeltaTime)
{
	if (CurrentTime < TimeToMove)
	{
		CurrentTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentTime / TimeToMove, 0.0f, 1.0f);
		const float MovementAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);

		switch (OpenStyle)
		{
			case EDoorOpenStyle::ROTATE:
			{
				const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, MovementAlpha);
				GetOwner()->SetActorRotation(CurrentRotation);

				if (CurrentRotation.Equals(FinalRotation, 1.0f))
				{
					DoorState = EDoorState::OPEN;
					CurrentTime = 0.0f;
				}

				if (DoorState == EDoorState::OPEN)
				{
					UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::OpeningDoor - Door rotated open"));
				}

				break;
			}
			case EDoorOpenStyle::SLIDE:
			{
				const FVector CurrentPosition = FMath::Lerp(StartPosition, FinalPosition, MovementAlpha);
				GetOwner()->SetActorLocation(CurrentPosition);

				if (CurrentPosition.Equals(FinalPosition, 1.0f))
				{
					DoorState = EDoorState::OPEN;
					CurrentTime = 0.0f;
				}

				if (DoorState == EDoorState::OPEN)
				{
					UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::OpeningDoor - Door slid open"));
				}

				break;
			}
		}
	}
}

void UDoorInteraction_Comp::ClosingDoor(float DeltaTime)
{
	if (CurrentTime < TimeToClose)
	{
		CurrentTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentTime / TimeToClose, 0.0f, 1.0f);
		const float MovementAlpha = CloseCurve.GetRichCurveConst()->Eval(TimeRatio);

		switch (OpenStyle)
		{
			case EDoorOpenStyle::ROTATE:
			{
				const FRotator CurrentRotation = FMath::Lerp(FinalRotation, StartRotation, MovementAlpha);
				GetOwner()->SetActorRotation(CurrentRotation);

				if (CurrentRotation.Equals(StartRotation, 1.0f))
				{
					DoorState = EDoorState::CLOSED;
					CurrentTime = 0.0f;
				}

				if (DoorState == EDoorState::CLOSED)
				{
					UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::ClosingDoor - Door rotated closed"));
				}

				break;
			}
			case EDoorOpenStyle::SLIDE:
			{
				const FVector CurrentPosition = FMath::Lerp(FinalPosition, StartPosition, MovementAlpha);
				GetOwner()->SetActorLocation(CurrentPosition);

				if (CurrentPosition.Equals(StartPosition, 1.0f))
				{
					DoorState = EDoorState::CLOSED;
					CurrentTime = 0.0f;
				}

				if (DoorState == EDoorState::CLOSED)
				{
					UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::ClosingDoor - Door slid closed"));
				}

				break;
			}
		}
	}
}

// Called when the game starts
void UDoorInteraction_Comp::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();

	if (Owner)
	{
		InitialRotation = Owner->GetActorRotation();
		StartRotation = InitialRotation;
		FinalRotation = Owner->GetActorRotation() + DesiredRotation;

		InitialPosition = Owner->GetActorLocation();
		StartPosition = InitialPosition;
		FinalPosition = Owner->GetActorLocation() + DesiredOffset;
	}

	if (TriggerBox)
	{
		UBoxComponent* BoxComp = TriggerBox->FindComponentByClass<UBoxComponent>();
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &UDoorInteraction_Comp::TriggerBoxEntered);
	}

	if (KeyTrigger)
	{
		UBoxComponent* KeyBoxComp = KeyTrigger->FindComponentByClass<UBoxComponent>();
		KeyBoxComp->OnComponentBeginOverlap.AddDynamic(this, &UDoorInteraction_Comp::KeyTriggerEntered);
	}

	CurrentTime = 0.0f;
}


// Called every frame
void UDoorInteraction_Comp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsLocked)
	{
		switch (DoorState)
		{
			case EDoorState::CLOSING:
			{
				ClosingDoor(DeltaTime);

				break;
			}
			case EDoorState::CLOSED:
			{
				break;
			}
			case EDoorState::OPENING:
			{
				OpeningDoor(DeltaTime);

				break;
			}
			case EDoorState::OPEN:
			{
				if (bCloses)
				{
					DoorState = EDoorState::WAITING;
				}

				break;
			}
			case EDoorState::WAITING:
			{
				if ((CurrentTime < TimeUntilClose))
				{
					CurrentTime += DeltaTime;
				}
				else
				{
					CurrentTime = 0.0f;
					DoorState = EDoorState::CLOSING;
				}

				break;
			}
		}
	}
}

