// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Gun/ModularFirearmTypes.h"
#include "ModularFirearmInterface.generated.h"

class UGunPartDataBase;
class UGunStockData;
class UGunSightData;
class UGunMagazineData;
class UGunGripData;
class UGunMuzzleData;
class AModularFirearm;



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
	virtual void SetModularPart(EFirearmComponentType componentType, UGunPartDataBase* partName) {}
	/*
	* Changes the part's material at the index 0.
	* Skins are defined in the equipped data asset.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firearm")
	virtual void SetModularPartSkin(EFirearmComponentType componentType, const FString& skinName) {}
	/*
    * Changes the part's material at the index 0.
    * Skins are defined in the equipped data asset.
    */
    UFUNCTION(BlueprintCallable, Category = "Firearm")
    virtual void SetAllSkins(const FString& skinName) {}
    	
	UFUNCTION(BlueprintCallable, Category = "Firearm")
	virtual FModularFirearmAmmo GetAmmo() const {return FModularFirearmAmmo();}	

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
	/**
	 * @param newMode the firing mode you're trying to swap to.
	 * @return returns false if the firing mode is already set or if the mode is not available.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	virtual bool TryChangeFiringMode(EFiringMode newMode) { return false;}
	/**
	 * Will attempt to increment the firing mode to the next available firing mode.
	 * Example if only Auto and Burst are available:
	 *		Auto -> Burst -> Auto -> ...
	 * @return returns true if the firing mode was changed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	virtual bool TryIncrementFiringMode() {return false;}
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
