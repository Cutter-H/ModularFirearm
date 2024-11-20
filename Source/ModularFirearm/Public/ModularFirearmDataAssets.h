// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "ModularFirearmDataAssets.generated.h"

UENUM(BlueprintType)
enum EFirearmComponentType : uint8{
	Receiver	=	0,
	Barrel		=	1,
	Grip		=	2,
	Magazine	=	3,
	Sight		=	4,
	Stock		=	5,
	Muzzle		=	6,
	Num			=	7	UMETA(Hidden)
};

UENUM(BlueprintType)
enum EFiringMode {
	Automatic,
	SemiAutomatic,
	Burst
};

UENUM(BlueprintType)
enum ETargetingMode{
	FocalPoint,
	DirectionOfMuzzle,
	CursorLocation
};
/*
 *
 */


#pragma region Component Base
UCLASS(NotBlueprintable, HideDropdown)
class MODULARFIREARM_API UGunPartDataBase : public UDataAsset
{
	GENERATED_BODY()
public:
	/*
	* Mesh of the part.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TObjectPtr<UStaticMesh> Mesh;
	/*
	* The different materials that can be set for this part.
	* The material is changed on slot 0.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TMap<FString, UMaterialInterface*> ReceiverSkins;
	/*
	* Where on the receiver is this part attached.
	* Muzzle parts will attempt to attach to the barrel, but will fall back to the reciever.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FName AttachSocketName = "Attachment";

	/* 
	* Make the effect for the part 
	*/UFUNCTION(BlueprintCallable, Category = "GunPart|GAS")
	virtual FGameplayEffectSpec GetEffect(UAbilitySystemComponent* abilitySystem, UModularFirearmAttributeSet* attributes) const {
		return FGameplayEffectSpec();
	}

};

#pragma endregion
#pragma region Barrel
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunBarrelData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunBarrelData() { AttachSocketName = "Barrel"; }
	/*
	* This affects bullet spread during continuous fire. Such as changing spread based on 1st shot or 5th shot.
	* X determines shot number. Y determines spread.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	TObjectPtr<UCurveFloat> VolleySpread;
	/*
	* VolleySpread is multiplied by this to get the final bullet spread angles.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	float DefaultSpreadMultiplier = 1.f;
	/*
	* Niagara system when firing.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	class UNiagaraSystem* MuzzleFlash;
	/*
	* Audio played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	USoundBase* DefaultFiringSound;
	/*
	* For AI Noise
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	float DefaultNoise = 1.f;
	/*
	* Function for randomization or logic to be added when grabbing sound.
	*/UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	USoundBase* GetFiringSound() const;
	USoundBase* GetFiringSound_Implementation() const { return DefaultFiringSound; }

	UFUNCTION(BlueprintCallable, Category = "FirearmData")
	float GetVolleySpread(float volleyCount) const {
		if (IsValid(VolleySpread)) {
			return VolleySpread->GetFloatValue(volleyCount);
		}
		return 0.f;
	}
	virtual FGameplayEffectSpec GetEffect(UAbilitySystemComponent* abilitySystem, UModularFirearmAttributeSet* attributes) const override {
		// Create the effect and make it instant. We want this to modify base values.
		UGameplayEffect* effect = UGameplayEffect::StaticClass()->GetDefaultObject<UGameplayEffect>();
		effect->DurationPolicy = EGameplayEffectDurationType::Instant;

		// Add Spread Multiplier
		FGameplayModifierInfo SpreadMultiplier_Modifier;
		SpreadMultiplier_Modifier.Attribute = attributes->GetSpreadMultiplierAttribute();
		SpreadMultiplier_Modifier.ModifierOp = EGameplayModOp::Override;
		SpreadMultiplier_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(DefaultSpreadMultiplier));
		effect->Modifiers.Add(SpreadMultiplier_Modifier);

		// Add Noise Modifier
		FGameplayModifierInfo Noise_Modifier;
		Noise_Modifier.Attribute = attributes->GetNoiseAttribute();
		Noise_Modifier.ModifierOp = EGameplayModOp::Override;
		Noise_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(DefaultNoise));
		effect->Modifiers.Add(Noise_Modifier);

		FGameplayEffectContextHandle context = abilitySystem->MakeEffectContext();
		
		return FGameplayEffectSpec(effect, context, 1);
	}
};
#pragma endregion
#pragma region Muzzle
/* This class is effectively empty. This was created for a clear distinction of Barrel and Muzzle. Muzzle variables override the equipped Barrel variables. */
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunMuzzleData : public UGunBarrelData {
	GENERATED_BODY()
public:
	UGunMuzzleData() {
		AttachSocketName = "Muzzle";
	}
};
#pragma endregion
#pragma region Grip
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunGripData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunGripData() { AttachSocketName = "Grip"; }
	/*
	* The base haptic feedback class
	*/UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	TObjectPtr<UForceFeedbackEffect> HapticFeedback;
	/*
	* Changes the intensity of the haptic feedback based on firearm level.
	*/UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	float HapticIntensity = 1.f;
	virtual FGameplayEffectSpec GetEffect(UAbilitySystemComponent* abilitySystem, UModularFirearmAttributeSet* attributes) const override {
		// Create the effect and make it instant. We want this to modify base values.
		UGameplayEffect* effect = UGameplayEffect::StaticClass()->GetDefaultObject<UGameplayEffect>();
		effect->DurationPolicy = EGameplayEffectDurationType::Instant;

		// Add Haptic Intensity
		FGameplayModifierInfo HapticIntensity_Modifier;
		HapticIntensity_Modifier.Attribute = attributes->GetHapticIntensityAttribute();
		HapticIntensity_Modifier.ModifierOp = EGameplayModOp::Override;
		HapticIntensity_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(HapticIntensity));
		effect->Modifiers.Add(HapticIntensity_Modifier);

		FGameplayEffectContextHandle context = abilitySystem->MakeEffectContext();

		return FGameplayEffectSpec(effect, context, 1);
	}
};
#pragma endregion
#pragma region Magazine
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunMagazineData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunMagazineData() { AttachSocketName = "Magazine"; }
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TArray<TSubclassOf<AActor>> BulletClasses;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	float MaxAmmo = 30;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	float ReloadSpeed = 1;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	UAnimMontage* DefaultReloadMontage;
	/* Created function so randomization or logic can be added when grabbing montage. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	UAnimMontage* GetReloadMontage() const;
	UAnimMontage* GetReloadMontage_Implementation() const { return DefaultReloadMontage; }

	virtual FGameplayEffectSpec GetEffect(UAbilitySystemComponent* abilitySystem, UModularFirearmAttributeSet* attributes) const override {
		// Create the effect and make it instant. We want this to modify base values.
		UGameplayEffect* effect = UGameplayEffect::StaticClass()->GetDefaultObject<UGameplayEffect>();
		effect->DurationPolicy = EGameplayEffectDurationType::Instant;

		// Add Max Ammo
		FGameplayModifierInfo MaxAmmo_Modifier;
		MaxAmmo_Modifier.Attribute = attributes->GetMaxAmmoAttribute();
		MaxAmmo_Modifier.ModifierOp = EGameplayModOp::Override;
		MaxAmmo_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(MaxAmmo));
		effect->Modifiers.Add(MaxAmmo_Modifier);

		// Add Reload Speed
		FGameplayModifierInfo ReloadSpeed_Modifier;
		ReloadSpeed_Modifier.Attribute = attributes->GetReloadSpeedAttribute();
		ReloadSpeed_Modifier.ModifierOp = EGameplayModOp::Override;
		ReloadSpeed_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(ReloadSpeed));
		effect->Modifiers.Add(ReloadSpeed_Modifier);

		FGameplayEffectContextHandle context = abilitySystem->MakeEffectContext();

		return FGameplayEffectSpec(effect, context, 1);
	}
};
#pragma endregion
#pragma region Sight
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunSightData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunSightData() { AttachSocketName = "Sight"; }
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	float FOVZoom = 1.f;

	virtual FGameplayEffectSpec GetEffect(UAbilitySystemComponent* abilitySystem, UModularFirearmAttributeSet* attributes) const override {
		// Create the effect and make it instant. We want this to modify base values.
		UGameplayEffect* effect = UGameplayEffect::StaticClass()->GetDefaultObject<UGameplayEffect>();
		effect->DurationPolicy = EGameplayEffectDurationType::Instant;

		// Add FOV Zoom
		FGameplayModifierInfo MaxAmmo_Modifier;
		MaxAmmo_Modifier.Attribute = attributes->GetMaxAmmoAttribute();
		MaxAmmo_Modifier.ModifierOp = EGameplayModOp::Override;
		MaxAmmo_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(FOVZoom));
		effect->Modifiers.Add(MaxAmmo_Modifier);

		FGameplayEffectContextHandle context = abilitySystem->MakeEffectContext();

		return FGameplayEffectSpec(effect, context, 1);
	}
};
#pragma endregion
#pragma region Stock
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunStockData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunStockData() { AttachSocketName = "Stock"; }
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	TSubclassOf<UCameraShakeBase> CamShake;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	float CamShakeIntensity = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	float SwapSpeed = 1.f;
	
	virtual FGameplayEffectSpec GetEffect(UAbilitySystemComponent* abilitySystem, UModularFirearmAttributeSet* attributes) const override {
		// Create the effect and make it instant. We want this to modify base values.
		UGameplayEffect* effect = UGameplayEffect::StaticClass()->GetDefaultObject<UGameplayEffect>();
		effect->DurationPolicy = EGameplayEffectDurationType::Instant;

		// Add Cam Shake Intensity
		FGameplayModifierInfo CamShakeIntensity_Modifier;
		CamShakeIntensity_Modifier.Attribute = attributes->GetCamShakeIntensityAttribute();
		CamShakeIntensity_Modifier.ModifierOp = EGameplayModOp::Override;
		CamShakeIntensity_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(CamShakeIntensity));
		effect->Modifiers.Add(CamShakeIntensity_Modifier);

		// Add Swap Speed
		FGameplayModifierInfo SwapSpeed_Modifier;
		SwapSpeed_Modifier.Attribute = attributes->GetSwapSpeedAttribute();
		SwapSpeed_Modifier.ModifierOp = EGameplayModOp::Override;
		SwapSpeed_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(SwapSpeed));
		effect->Modifiers.Add(SwapSpeed_Modifier);

		FGameplayEffectContextHandle context = abilitySystem->MakeEffectContext();

		return FGameplayEffectSpec(effect, context, 1);
	}
};
#pragma endregion

