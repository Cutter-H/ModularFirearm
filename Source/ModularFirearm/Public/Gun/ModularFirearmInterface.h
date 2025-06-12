// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ModularFirearmDataAssets.h"
#include "ModularFirearmInterface.generated.h"

class UGunPartDataBase;
class UGunStockData;
class UGunSightData;
class UGunMagazineData;
class UGunGripData;
class UGunMuzzleData;
class AModularFirearm;

USTRUCT(BlueprintType)
struct FModularFirearmStats {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int CurrentAmmo;
	UPROPERTY(BlueprintReadWrite)
	int MaxAmmo;
	UPROPERTY(BlueprintReadWrite)
	int ReserveAmmo;
	UPROPERTY(BlueprintReadWrite)
	float BulletSpread;
	UPROPERTY(BlueprintReadWrite)
	float Noise;
	UPROPERTY(BlueprintReadWrite)
	float FireRate;
	UPROPERTY(BlueprintReadWrite)
	float HapticIntensity;
	UPROPERTY(BlueprintReadWrite)
	float CamShakeIntensity;
	UPROPERTY(BlueprintReadWrite)
	float Multishot;
	UPROPERTY(BlueprintReadWrite)
	float BurstSpeed;
	UPROPERTY(BlueprintReadWrite)
	int BurstAmount;
	UPROPERTY(BlueprintReadWrite)
	float ReloadSpeedMultiplier;

	
};

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UModularFirearmInterface : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class MODULARFIREARM_API IModularFirearmInterface {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, Category = "Firearm")
	virtual AModularFirearm* AsModularFirearm() {return nullptr;}
	/*
	* Changes the part data for the selected component.
	* Level is NOT utilized by default, but is passed to the effect. If you wish to use this please override MakeEffect to provide one that scales.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm")
	virtual bool SetModularPart(EFirearmComponentType componentType, UGunPartDataBase* partName, int level) {return false;}
	/*
	* Changes the part's material at the index 0.
	* Skins are defined in the equipped data asset.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm")
	virtual bool SetModularPartSkin(EFirearmComponentType componentType, const FString& skinName) {return false;}
	
	UFUNCTION(BlueprintCallable, Category = "Firearm")
	virtual FModularFirearmStats GetStats() const {return FModularFirearmStats();}


	/*
	* Starts firing the weapon.
	* Automatic FiringMode repeats FireWeapon.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	virtual void StartFiring() {}
	/*
	* Stops firing the weapon. 
	* This really only needs to be used for the Automatic FiringMode.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	virtual void StopFiring() {}
	/*
	* Begins the reload animation.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	virtual void Reload() {}
	/*
	* Attempts to cancels any active reload.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	virtual void StopReloading() {}
	/*
	* Sets the CurrentAmmo to MaxAmmo of the equipped magazine.
	* FreeFill does not reduce Reserve Ammo.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	virtual void LoadNewMagazine(bool freeFill = false) {}

	
	UFUNCTION(BlueprintCallable, Category = "Firearm|Parts")
	virtual UGunBarrelData* GetBarrel() const {return nullptr;}
	UFUNCTION(BlueprintCallable, Category = "Firearm|Parts")
	virtual UGunMuzzleData* GetMuzzle() const {return nullptr;}
	UFUNCTION(BlueprintCallable, Category = "Firearm|Parts")
	virtual UGunGripData* GetGrip() const {return nullptr;}
	UFUNCTION(BlueprintCallable, Category = "Firearm|Parts")
	virtual UGunMagazineData* GetMagazine() const {return nullptr;}
	UFUNCTION(BlueprintCallable, Category = "Firearm|Parts")
	virtual UGunSightData* GetSight() const {return nullptr;}
	UFUNCTION(BlueprintCallable, Category = "Firearm|Parts")
	virtual UGunStockData * GetStock() const {return nullptr;}

	
	
};
