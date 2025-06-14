// Cutter Hodnett // 2024-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "ModularFirearmDataAssets.generated.h"

class UGameplayEffect;
class UModularFirearmAttributeSet;

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
	TMap<FString, TSoftObjectPtr<UMaterialInterface>> Skins;
	/*
	* Where on the receiver is this part attached.
	* Muzzle parts will attempt to attach to the barrel, but will fall back to the reciever.
	*/UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FName AttachSocketName = "Attachment";

	/* 
	* Make the effect for the part 
	*/UFUNCTION(BlueprintCallable, Category = "GunPart|GAS")
	virtual UGameplayEffect* MakeEffect(UModularFirearmAttributeSet* attributes) const;
	/*
	 * Returns false if all assets are already loaded.
	 */UFUNCTION(BlueprintCallable, Category = "GunPart")
	virtual void LoadAssets();
protected:
	UFUNCTION()
	virtual TArray<FSoftObjectPath> GetAssetPaths() const;
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
	float DefaultSpreadMultiplier = 1.f;
	/*
	* Niagara system when firing.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	TSoftObjectPtr<class UNiagaraSystem> MuzzleFlash;
	/*
	* Audio played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	TSoftObjectPtr<USoundBase> DefaultFiringSound;
	/*
	* For AI Noise
	*/UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	float DefaultNoise = 1.f;
	/*
	* Function for randomization or logic to be added when grabbing sound.
	*/UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	TSoftObjectPtr<USoundBase> GetFiringSound() const;
	TSoftObjectPtr<USoundBase> GetFiringSound_Implementation() const { return DefaultFiringSound; }

	UFUNCTION(BlueprintCallable, Category = "FirearmData")
	float GetVolleySpread(float volleyCount) const {
		if (IsValid(VolleySpread)) {
			return VolleySpread->GetFloatValue(volleyCount);
		}
		return 0.f;
	}
	virtual UGameplayEffect* MakeEffect(UModularFirearmAttributeSet* attributes) const override;
protected:
	virtual TArray<FSoftObjectPath> GetAssetPaths() const;
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
	virtual UGameplayEffect* MakeEffect(UModularFirearmAttributeSet* attributes) const override;
protected:
	virtual TArray<FSoftObjectPath> GetAssetPaths() const override;
};



UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunMagazineData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunMagazineData() { AttachSocketName = "Magazine"; }
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TArray<TSoftClassPtr<AActor>> BulletClasses;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	float MaxAmmo = 30;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	float ReloadSpeed = 1;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TSoftObjectPtr<UAnimMontage> DefaultReloadMontage;
	/* Created function so randomization or logic can be added when grabbing montage. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	TSoftObjectPtr<UAnimMontage> GetReloadMontage() const;
	TSoftObjectPtr<UAnimMontage> GetReloadMontage_Implementation() const { return DefaultReloadMontage; }

	virtual UGameplayEffect* MakeEffect(UModularFirearmAttributeSet* attributes) const override;
protected:
	virtual TArray<FSoftObjectPath> GetAssetPaths() const override;
};

UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunSightData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunSightData() { AttachSocketName = "Sight"; }
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	float FOVZoom = 1.f;

	virtual UGameplayEffect* MakeEffect(UModularFirearmAttributeSet* attributes) const override;
};

UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunStockData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunStockData() { AttachSocketName = "Stock"; }
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	TSoftClassPtr<UCameraShakeBase> CamShake;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	float CamShakeIntensity = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	float SwapSpeed = 1.f;
	
	virtual UGameplayEffect* MakeEffect(UModularFirearmAttributeSet* attributes) const override;
protected:
	virtual TArray<FSoftObjectPath> GetAssetPaths() const override;
};

