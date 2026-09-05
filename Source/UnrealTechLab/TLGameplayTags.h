#pragma once

#include "NativeGameplayTags.h"

namespace TLGameplayTags
{	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_SwordAndShield_NormalAttack);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Bow_NormalShot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Bow_MultiShot);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_ThrowingKnife);
	

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_SwordAndShield_NormalAttack);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_Bow_NormalShot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_Bow_MultiShot);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_ThrowingKnife);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_SpawnProjectile);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_Nocked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Attack_MeleeHit);
	// Test Update
}