#pragma once

#include "CoreMinimal.h"
#include "TLInputConfig.h"
#include "EnhancedInputComponent.h"
#include "TLInputComponent.generated.h"

struct FGameplayTag;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALTECHLAB_API UTLInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:	
	/** InputTag에 해당하는 기본 입력 액션을 TriggerEvent에 맞게 바인딩하는 함수입니다.
	 * 뒤에 Vars를 넘기면 핸들러 호출 시 그대로 전달됩니다. (예: QuickBar 슬롯 인덱스)
	 */
	template <class UserClass, typename FuncType, typename... VarTypes>
	void BindNativeAction(const UTLInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, VarTypes... Vars);

	/** AbilityInputActions 전체를 Triggered/Completed로 일괄 바인딩하고 핸들을 BindHandles에 저장하는 함수입니다.
	 * BindHandles는 런타임 중 등록한 어빌리티를 제거하기 위한 용도입니다.
	 */
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityAction(const UTLInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);

	/** InputTag에 해당하는 UI 입력 액션을 Started TriggerEvent에 바인딩하는 함수입니다. */
	template <class UserClass, typename FuncType>
	void BindUIAction(const UTLInputConfig* InputConfig, UserClass* Object, FuncType Func);

};

template <class UserClass, typename FuncType, typename... VarTypes>
void UTLInputComponent::BindNativeAction(const UTLInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, VarTypes... Vars)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func, Vars...);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UTLInputComponent::BindAbilityAction(const UTLInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);
	
	for (const FTLInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag).GetHandle();
			}
			
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle();
			}
		}
	}
}

template <class UserClass, typename FuncType>
void UTLInputComponent::BindUIAction(const UTLInputConfig* InputConfig, UserClass* Object, FuncType Func)
{
	check(InputConfig);
    
	for (const FTLInputAction& Action : InputConfig->UIInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, Func, Action.InputTag);
		}
	}
}