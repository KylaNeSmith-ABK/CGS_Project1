// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "InteractableDoor.generated.h"

class UDoorInteraction_Comp;

UCLASS()
class CGS_PROJECT1_API AInteractableDoor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AInteractableDoor();

protected:
	UPROPERTY(EditAnywhere, NoClear)
	UDoorInteraction_Comp* DoorInteractionComp = nullptr;
};
