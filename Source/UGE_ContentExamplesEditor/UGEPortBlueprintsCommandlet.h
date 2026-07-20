#pragma once

#include "Commandlets/Commandlet.h"
#include "UGEPortBlueprintsCommandlet.generated.h"

UCLASS()
class UGE_CONTENTEXAMPLESEDITOR_API UGEPortBlueprintsCommandlet final : public UCommandlet
{
	GENERATED_BODY()

public:
	UGEPortBlueprintsCommandlet();
	virtual int32 Main(const FString& Params) override;
};
