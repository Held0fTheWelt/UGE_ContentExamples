// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animation/SubmarineAnimInstance.h"

USubmarineAnimInstance::USubmarineAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USubmarineAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	PropellerYaw = 0.f;
	DivePitch = 0.f;
}

void USubmarineAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	PropellerYaw += PropulsionSpeed * 360.f * DeltaSeconds;
	PropellerYaw = FMath::Fmod(PropellerYaw, 360.f);
	DivePitch = FMath::Clamp(DivePitch + DiveRate * DeltaSeconds, -45.f, 45.f);
}

void USubmarineAnimInstance::SetPropulsionSpeed(float Value)
{
	PropulsionSpeed = Value;
}

void USubmarineAnimInstance::SetDiveRate(float Value)
{
	DiveRate = Value;
}

void USubmarineAnimInstance::SetRudderInput(float Value)
{
	RudderInput = Value;
}
