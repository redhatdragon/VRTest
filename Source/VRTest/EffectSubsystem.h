// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Flatbuffer.h"
#include "GAME_LIMITS.h"
#include "HealthComponent.h"
#include "IOFunctionLibrary.h"
#include "EffectSubsystem.generated.h"

UCLASS()
class VRTEST_API UEffectSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
	GENERATED_BODY()
	// Unordered buffer of subscribed AActor pointers
	FlatBuffer<AActor*, GAME_LIMITS::MAX_ACTOR_COUNT> subbedActors;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override {
		
	}
	virtual void Deinitialize() override {
		
	}
	virtual void BeginDestroy() override {
		
	}
	//virtual bool IsTickableInEditor() const override { return true; };
	//virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override {
		return GetStatID();
	}
	virtual void Tick(float DeltaTime) override {
		uint32_t count = subbedActors.count;
		//TODO: easy location to potentially multithread later
		for (uint32_t i = 0; i < count; i++) {
			AActor* actor = subbedActors[i];
			//auto component = actor->GetComponentByClass<UActorComponent>();
			UHealthComponent* healthComponent = (UHealthComponent*)actor->GetComponentByClass(UHealthComponent::StaticClass());
			uint32_t effectsCount = healthComponent->Effects.count;
			if(effectsCount == 0) {
				UnsubActor(actor);
				continue;
			}
			auto& effects = healthComponent->Effects;
			for (uint32_t j = 0; j < effectsCount; j++) {
				effects[j].TicksLeft--;
				if (effects[j].EffectType == EEffect_DOT) {
					tickDOT(healthComponent, &effects[j]);

					FString str = "DOT effect ticked...  Remaining health: ";
					str += FString::FromInt(healthComponent->Stats.HitPoints);
					UIOFunctionLibrary::printMsg(str);
				}
				if(effects[j].TicksLeft == 0) {
					effectsCount = effects.count--;
					effects[j] = effects[effectsCount];
				}
			}
		}
	}

	void SubActor(AActor* actor) {
		subbedActors.push(actor);
	}
	void UnsubActor(AActor* actor) {
		uint32_t count = subbedActors.count;
		for (uint32_t i = 0; i < count; i++) {
			if(subbedActors[i] == actor) {
				subbedActors[i] = subbedActors[count-1];
				subbedActors.pop();
				return;
			}
		}
	}
private:
	void tickDOT(UHealthComponent* healthComponent, FEffectData* effectData) {
		healthComponent->Damage(effectData->Value, effectData->ElementType);
	}
};
