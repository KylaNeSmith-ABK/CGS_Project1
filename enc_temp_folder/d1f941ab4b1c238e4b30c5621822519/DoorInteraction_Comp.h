// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "DoorInteraction_Comp.generated.h"

class ATriggerBox;
class AStaticMeshActor;

UENUM()
enum class EDoorOpenStyle
{
	ROTATE,
	SLIDE
};

UENUM()
enum class EDoorState
{
	CLOSING,
	CLOSED,
	OPENING,
	OPEN,
	WAITING
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

	UFUNCTION()
	void TriggerBoxEntered(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void KeyTriggerEntered(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	void OpeningDoor(float DeltaTime);
	void ClosingDoor(float DeltaTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// -- HOW THE DOOR WILL OPEN --
	UPROPERTY(EditAnywhere)
	EDoorOpenStyle OpenStyle = EDoorOpenStyle::ROTATE;

	UPROPERTY(EditAnywhere, meta=(EditCondition="OpenStyle==EDoorOpenStyle::ROTATE"))
	FRotator DesiredRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, meta=(EditCondition="OpenStyle==EDoorOpenStyle::SLIDE"))
	FVector DesiredOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	ATriggerBox* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere)
	AStaticMeshActor* FrameMesh = nullptr;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve OpenCurve;

	UPROPERTY(EditAnywhere)
	float TimeToMove = 1.0f;

	// -- DOOR KEY USAGE --
	UPROPERTY(EditAnywhere)
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsLocked"))
	ATriggerBox* KeyTrigger = nullptr;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsLocked"))
	AStaticMeshActor* KeyMesh = nullptr;

	// -- DOOR CLOSING --
	UPROPERTY(EditAnywhere)
	bool bCloses = false;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCloses"))
	FRuntimeFloatCurve CloseCurve;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCloses"))
	float TimeUntilClose = 1.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCloses"))
	float TimeToClose = 1.0f;
	
	float CurrentTime = 0.0f;

	// -- FOR INTERNAL USAGE --
	FRotator InitialRotation = FRotator::ZeroRotator;
	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;

	FVector InitialPosition = FVector::ZeroVector;
	FVector StartPosition = FVector::ZeroVector;
	FVector FinalPosition = FVector::ZeroVector;


	EDoorState DoorState = EDoorState::CLOSED;

	//bool bIsOpen = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
