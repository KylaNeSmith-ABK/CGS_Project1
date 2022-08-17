// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteraction_Comp.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UDoorInteraction_Comp::UDoorInteraction_Comp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDoorInteraction_Comp::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();

	if (Owner)
	{
		StartRotation = Owner->GetActorRotation();
		FinalRotation = Owner->GetActorRotation() + DesiredRotation;

		StartPosition = Owner->GetActorLocation();
		FinalPosition = Owner->GetActorLocation() + DesiredOffset;
	}

	CurrentTime = 0.0f;

	bIsLocked = bUsesKey;
}


// Called every frame
void UDoorInteraction_Comp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsOpen && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

		if (KeyTrigger && KeyTrigger->IsOverlappingActor(PlayerPawn))
		{
			bIsLocked = false;
			KeyTrigger->Destroy();

			if (KeyMesh)
			{
				KeyMesh->Destroy();
			}
		}

		if (!bIsLocked && PlayerPawn && TriggerBox && (CurrentTime < TimeToMove) && TriggerBox->IsOverlappingActor(PlayerPawn))
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

					bIsOpen = CurrentRotation.Equals(FinalRotation, 1.0f);

					if (bIsOpen)
					{
						UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::TickComponent - Door rotated open"));
					}

					break;
				}
				case EDoorOpenStyle::SLIDE:
				{
					const FVector CurrentPosition = FMath::Lerp(StartPosition, FinalPosition, MovementAlpha);
					GetOwner()->SetActorLocation(CurrentPosition);

					bIsOpen = CurrentPosition.Equals(FinalPosition, 1.0f);

					if (bIsOpen)
					{
						UE_LOG(LogTemp, Warning, TEXT("DoorInteraction_Comp.cpp::TickComponent - Door slid open"));
					}

					break;
				}
			}
		}
	}
	
}

