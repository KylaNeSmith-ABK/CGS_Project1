// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableDoor.h"
#include "DoorInteraction_Comp.h"

AInteractableDoor::AInteractableDoor()
{
	DoorInteractionComp = CreateDefaultSubobject<UDoorInteraction_Comp>(TEXT("DoorInteractionComponent"));
}
