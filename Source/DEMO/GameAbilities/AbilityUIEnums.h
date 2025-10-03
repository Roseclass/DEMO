#pragma once

#include "AbilityUIEnums.generated.h"

//#include "GameAbilities/AbilityUIEnums.h"

UENUM(BlueprintType)
enum class ESkillSlotLocation : uint8
{
	Attack,	Skill_0,Skill_1,Passive,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESkillListLocation : uint8
{
	Skill_00, Skill_01, 
	Skill_10, Skill_11, 
	Skill_20, Skill_21, 
	Skill_30, Skill_31, 
	MAX UMETA(Hidden)
};
