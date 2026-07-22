#pragma once

#include "CoreMinimal.h"
#include "LyraContentCompatibilityTypes.generated.h"

/**
 * LyraGame module residual surface (S6.5).
 * Living UCLASS shells removed — ObserveTeamColors → Elemental_Teams,
 * LyraLobbyBackground → Core_Classes.CoreLobbyBackground (ClassRedirect).
 * Marker struct keeps the module's generated header valid for empty public API.
 */
USTRUCT()
struct FLyraContentCompatibilityMarker
{
	GENERATED_BODY()
};
