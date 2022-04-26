// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlatBuffer.h"
#include "HealthComponent.generated.h"

UENUM()
enum EElement_Types {
	EElement_True,
	EElement_Physical,
	EElement_Fire,
	EElement_Ice
}; constexpr uint32_t EElement_Count = 4;

USTRUCT(BlueprintType)
struct VRTEST_API FHealthStats {
	GENERATED_BODY()
	uint32 HitPoints;
	float Resistance[EElement_Count];  //TODO: make these fixed point.
};


UENUM()
enum EEffect_Types {
	EEffect_Nothing,
	EEffect_DOT,  // damage over time
	EEffect_HOT,  // healing over time
}; constexpr uint32_t EEffect_Count = 3;

USTRUCT(BlueprintType)
struct VRTEST_API FEffectData {
	GENERATED_BODY()
	EEffect_Types EffectType;
	//TODO: consider making this into an array
	uint32 Value, Range, TicksLeft;
	EElement_Types ElementType;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRTEST_API UHealthComponent : public UActorComponent {
	GENERATED_BODY()
public:
	FHealthStats Stats;
	FlatBuffer<FEffectData, 100> Effects;
	UHealthComponent() {
		Stats = {100, {1, 1, 1, 1}};
		PrimaryComponentTick.bCanEverTick = false;
	}
	// Value must be positive only.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void DamageRaw(int32 Value) {
		uint32 value = Value;
		uint32 newHitPoints = Stats.HitPoints-value;
		if (newHitPoints >= Stats.HitPoints) {
			Stats.HitPoints = 0;
			GetOwner()->Destroy();  //TODO: watch for implications
		}
		Stats.HitPoints = newHitPoints;
	}
	// Value must be positive only.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Damage(int32 Value, EElement_Types elementType) {
		uint32 damage = ((float)Value)*Stats.Resistance[elementType];
		DamageRaw(damage);
	}
	// All args use non negative numbers only.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetStats(int32 HitPoints, float TrueResist,
		float PhysicalResist, float FireResist, float IceResist)
	{
		Stats.HitPoints = HitPoints;
		SetResistances(TrueResist, PhysicalResist, FireResist, IceResist);
	}
	// Arg uses non negatives only.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHitPoints(int32 HitPoints) {
		Stats.HitPoints = HitPoints;
	}
	// All args use non negative numbers only.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetResistances(float TrueResist, float PhysicalResist,
		float FireResist, float IceResist)
	{
		Stats.Resistance[EElement_True] = TrueResist;
		Stats.Resistance[EElement_Physical] = PhysicalResist;
		Stats.Resistance[EElement_Fire] = FireResist;
		Stats.Resistance[EElement_Ice] = IceResist;
	}

	// Args must be non negative.
	UFUNCTION(BlueprintCallable, Category = "Health | Effect")
	void AddEffectDOT(int32 Value, EElement_Types ElementType, int32 Ticks);
	// Args must be non negative.
	UFUNCTION(BlueprintCallable, Category = "Health | Effect")
	void AddEffectDOTIfUnique(int32 Value, EElement_Types ElementType, int32 Ticks);
	// Args must be non negative.  TODO: Yeah this seems overly long winded.
	UFUNCTION(BlueprintCallable, Category = "Health | Effect")
	void AddEffectDOTOverrideFirstIfDup(int32 Value, EElement_Types ElementType, int32 Ticks);
protected:
	// Called when the game starts
	virtual void BeginPlay() override {
		Super::BeginPlay();
	}
};
