// just an agent gameplay tags instead of creating them in Project Settings

#pragma once

#include "NativeGameplayTags.h"

namespace AgentGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TeaState);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TeaState_Wait);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TeaState_Walk);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TeaState_Rush);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_NonBattleState);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_NonBattleState_Wait);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_NonBattleState_Patrol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_NonBattleState_Rush);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_BattleState);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_BattleState_Chase);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonTeaState_BattleState_Attack);

}