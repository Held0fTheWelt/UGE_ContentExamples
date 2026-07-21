// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SubmarineAnimInstance.generated.h"

/**
 * AnimInstance contract for submarine skeletal meshes (equipment-family ring target).
 *
 * Provides Blueprint-readable dive / propulsion inputs that AnimBP graphs bind to
 * for composition; implementation stays in C++ / AnimBP authorable layers only.
 */
UCLASS()
class SUBMARINES_API USubmarineAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USubmarineAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Submarine Animation|Input")
	float PropulsionSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Submarine Animation|Input")
	float DiveRate = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Submarine Animation|Input")
	float RudderInput = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Submarine Animation|Computed")
	float PropellerYaw = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Submarine Animation|Computed")
	float DivePitch = 0.f;

public:
	void SetPropulsionSpeed(float Value);
	void SetDiveRate(float Value);
	void SetRudderInput(float Value);
};
