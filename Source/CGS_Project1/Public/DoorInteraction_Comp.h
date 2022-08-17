// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "DoorInteraction_Comp.generated.h"

class ATriggerBox;

UENUM()
enum class EDoorOpenStyle
{
	ROTATE,
	SLIDE
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CGS_PROJECT1_API UDoorInteraction_Comp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorInteraction_Comp();

private:
	AActor* Owner = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	EDoorOpenStyle OpenStyle = EDoorOpenStyle::ROTATE;

	UPROPERTY(EditAnywhere, meta=(EditCondition="OpenStyle==EDoorOpenStyle::ROTATE"))
	FRotator DesiredRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, meta=(EditCondition="OpenStyle==EDoorOpenStyle::SLIDE"))
	FVector DesiredOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bUsesKey = false;

	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;

	FVector StartPosition = FVector::ZeroVector;
	FVector FinalPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float TimeToMove = 1.0f;
	
	float CurrentTime = 0.0f;

	UPROPERTY(EditAnywhere)
	ATriggerBox* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUsesKey"))
	ATriggerBox* KeyTrigger = nullptr;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve OpenCurve;

	bool bIsOpen = false;

	bool bIsLocked = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
