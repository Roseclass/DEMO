#pragma once

#include "AbilityEnums.generated.h"

//#include "GameAbilities/AbilityEnums.h"

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
    Attack,
    Skill,
    Passive,
    MAX
};
