// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "EffectSubsystem.h"

void UHealthComponent::AddEffectDOT(int32 Value, EElement_Types ElementType, int32 Ticks) {
	FEffectData effectData;
	effectData.EffectType = EEffect_Types::EEffect_DOT;
	effectData.Value = Value;
	effectData.Range = 0;
	effectData.ElementType = ElementType;
	effectData.TicksLeft = Ticks;
	Effects.push(effectData);
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UEffectSubsystem* effectSubsystem = GameInstance->GetSubsystem<UEffectSubsystem>();
	effectSubsystem->SubActor(GetOwner());
}
void UHealthComponent::AddEffectDOTIfUnique(int32 Value, EElement_Types ElementType, int32 Ticks) {
	uint32_t count = Effects.count;
	for (uint32_t i = 0; i < count; i++) {
		if (Effects[i].EffectType == EEffect_DOT)
			if (ElementType == Effects[i].ElementType)
				return;
	}
	AddEffectDOT(Value, ElementType, Ticks);
}
void UHealthComponent::AddEffectDOTOverrideFirstIfDup(int32 Value, EElement_Types ElementType, int32 Ticks) {
	uint32_t count = Effects.count;
	for (uint32_t i = 0; i < count; i++) {
		if (Effects[i].EffectType == EEffect_DOT)
			if (ElementType == Effects[i].ElementType) {
				Effects[i].Value = Value;
				Effects[i].TicksLeft = Ticks;
			}
	}
	AddEffectDOT(Value, ElementType, Ticks);
}