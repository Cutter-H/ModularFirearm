// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ModularFirearmAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class MODULARFIREARM_API UModularFirearmAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
#pragma region Attribute Accessors
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, SpreadMultiplier);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, Noise);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, CamShakeIntensity);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, HapticIntensity);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, MaxAmmo);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, ReloadSpeed);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, FOVZoom);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, SwapSpeed);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, Multishot);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, FireRate);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, BurstSpeed);
	ATTRIBUTE_ACCESSORS(UModularFirearmAttributeSet, BurstAmount);
#pragma endregion
protected:
#pragma region OnRep Functions
	UFUNCTION()
	virtual void OnRep_SpreadMultiplier(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_Noise(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_RecoilMultiplier(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_HapticIntensity(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_MaxAmmo(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_ReloadSpeed(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_FOVZoom(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_SwapSpeed(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_Multishot(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_FireRate(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_BurstSpeed(const FGameplayAttributeData& oldAttribute) const;
	UFUNCTION()
	virtual void OnRep_BurstAmount(const FGameplayAttributeData& oldAttribute) const;
#pragma endregion
private:
#pragma region Attributes
	/*
	* Base determined by Barrel
	* VolleySpread is multiplied by this to get the final bullet spread angles.
	*/UPROPERTY(ReplicatedUsing = OnRep_SpreadMultiplier)
	FGameplayAttributeData SpreadMultiplier;
	/*
	* Dictates noise on weapon firing. This is commonly used in AI. Note that this is NOT SFX volume.
	* Base determined by Barrel
	*/UPROPERTY(ReplicatedUsing = OnRep_Noise)
	FGameplayAttributeData Noise;
	/*
	* Affects the scale of haptic feedback in controllers when firing the gun.
	* Base determined by Grip
	*/UPROPERTY(ReplicatedUsing = OnRep_HapticIntensity)
	FGameplayAttributeData HapticIntensity;
	/*
	* Changes the max amount of bullets per magazine.
	* [Truncated to int]
	* Base determined by Magazine
	*/UPROPERTY(ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	/*
	* Affects the rate of the reload speed montage. 1 is normal rate.
	* Base determined by Magazine
	*/UPROPERTY(ReplicatedUsing = OnRep_ReloadSpeed)
	FGameplayAttributeData ReloadSpeed;
	/*
	* Affects the player's camera when using ADS.
	* Base determined by Sight
	*/UPROPERTY(ReplicatedUsing = OnRep_FOVZoom)
	FGameplayAttributeData FOVZoom;
	/*
	* Affects the scale if the CamShake used on firing.
	* Base determined by Stock
	*/UPROPERTY(ReplicatedUsing = OnRep_RecoilMultiplier)
	FGameplayAttributeData CamShakeIntensity;
	/*
	* Alters how fast the firearm can be swapped from/to.
	* Base determined by Stock
	*/UPROPERTY(ReplicatedUsing = OnRep_SwapSpeed)
	FGameplayAttributeData SwapSpeed;
	/*
	* Increases the number of additional bullets used when firing the weapon. (Useful for shotguns)
	* Base determined by Receiver
	*/UPROPERTY(ReplicatedUsing = OnRep_Multishot)
	FGameplayAttributeData Multishot;
	/*
	* How fast bullets can be fired.
	* Base determined by Receiver
	*/UPROPERTY(ReplicatedUsing = OnRep_FireRate)
	FGameplayAttributeData FireRate;
	/*
	* How fast the bullets in each burst are fired.
	* Base determined by Receiver
	*/UPROPERTY(ReplicatedUsing = OnRep_BurstSpeed)
	FGameplayAttributeData BurstSpeed;
	/*
	* How many bullets can be fired in a single burst
	* [Truncated to int]
	* Base determined by Receiver
	*/UPROPERTY(ReplicatedUsing = OnRep_BurstAmount)
	FGameplayAttributeData BurstAmount;
#pragma endregion
};
