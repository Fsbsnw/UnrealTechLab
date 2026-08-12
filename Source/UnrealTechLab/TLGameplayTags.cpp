#include "TLGameplayTags.h"

namespace TLGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look_Mouse, "InputTag.Look.Mouse");

	// Input Tag
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_SwordAndShield_NormalAttack, "InputTag.Ability.SwordAndShield.NormalAttack");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Bow_NormalShot, "InputTag.Ability.Bow.NormalShot");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Bow_MultiShot, "InputTag.Ability.Bow.MultiShot");
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_ThrowingKnife, "InputTag.Ability.ThrowingKnife");


	// Ability Tag
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_ThrowingKnife, "Ability.Skill.ThrowingKnife");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_Bow_NormalShot, "Ability.Skill.Bow.NormalShot");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_Bow_MultiShot, "Ability.Skill.Bow.MultiShot");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_SwordAndShield_NormalAttack, "Ability.Skill.SwordAndShield.NormalAttack");

	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Attack_SpawnProjectile, "GameplayEvent.Attack.SpawnProjectile");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Attack_Nocked, "GameplayEvent.Attack.Nocked");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Attack_MeleeHit, "GameplayEvent.Attack.MeleeHit");
}