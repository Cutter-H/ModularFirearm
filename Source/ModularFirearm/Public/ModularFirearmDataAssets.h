// Cutter Hodnett // 2024-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gun/ModularFirearmTypes.h"
#include "ModularFirearmDataAssets.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGunPartAssetsLoaded, UGunPartDataBase*, parData);
/*
 *
 */
UCLASS(NotBlueprintable, HideDropdown)
class MODULARFIREARM_API UGunPartDataBase : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "GunPart")
	FOnGunPartAssetsLoaded OnGunPartAssetsLoaded;
	/*
	* Mesh of the part.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TSoftObjectPtr<UStaticMesh> Mesh;
	/*
	* The different materials that can be set for this part.
	* The material is changed on slot 0.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TMap<FString, FFirearmMaterialArray> Skins;
	/*
	* Where on the receiver is this part attached.
	* Muzzle parts will attempt to attach to the barrel, but will fall back to the reciever.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FName AttachSocketName = "Attachment";
};
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
	float BulletSpreadMultiplier = 1.f;
	/*
	* Niagara system when firing.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	TSoftObjectPtr<class UNiagaraSystem> MuzzleFlash;
	/*
	* Audio played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	TSoftObjectPtr<USoundBase> FiringSound;
	/*
	* For AI Noise
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	float Noise = 1.f;

	UFUNCTION(BlueprintCallable, Category = "FirearmData")
	float GetVolleySpread(float volleyCount) const {
		if (IsValid(VolleySpread)) {
			return BulletSpreadMultiplier * VolleySpread->GetFloatValue(volleyCount);
		}
		return 0.f;
	}
};
/* This class is effectively empty. This was created for a clear distinction of Barrel and Muzzle. Muzzle variables override the equipped Barrel variables. */
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunMuzzleData : public UGunBarrelData {
	GENERATED_BODY()
public:
	UGunMuzzleData() { AttachSocketName = "Muzzle"; }
};
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
};



UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunMagazineData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunMagazineData() { AttachSocketName = "Magazine"; }
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TArray<TSubclassOf<AActor>> BulletClasses;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	int MaxAmmo = 30;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	float ReloadSpeed = 1;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TObjectPtr<UAnimMontage> ReloadMontage;
	/* Created function so randomization can be added when grabbing montage. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	UAnimMontage* GetReloadMontage() const;
	UAnimMontage* GetReloadMontage_Implementation() const { return ReloadMontage; }
};

UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunSightData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunSightData() { AttachSocketName = "Sight"; }
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	float FOVZoom = 1.f;
};

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
};

