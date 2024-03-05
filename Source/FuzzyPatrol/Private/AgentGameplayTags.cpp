// Definition file of Agent Gameplay Tags


#include "AgentGameplayTags.h"

namespace AgentGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(TeaReceivingState, "TeaReceivingState");

	UE_DEFINE_GAMEPLAY_TAG(NonTeaState, "NonTeaState");

	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_NonBattleState, "NonTeaState.NonBattleState");
	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_NonBattleState_Wait, "NonTeaState.NonBattleState.Wait");
	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_NonBattleState_Patrol, "NonTeaState.NonBattleState.Patrol");
	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_NonBattleState_Rush, "NonTeaState.NonBattleState.Rush");

	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_BattleState, "NonTeaState.BattleState");
	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_BattleState_Chase, "NonTeaState.BattleState.Chase");
	UE_DEFINE_GAMEPLAY_TAG(NonTeaState_BattleState_Attack, "NonTeaState.BattleState.Attack");

	// UE_DEFINE_GAMEPLAY_TAG_COMMENT()
}