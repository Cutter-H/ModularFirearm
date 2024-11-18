// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
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
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TObjectPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TMap<FString, UMaterialInterface*> ReceiverSkins;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FName AttachSocketName = "Attachment";

};

#pragma endregion
#pragma region Barrel
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunBarrelData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunBarrelData() { 
		AttachSocketName = "Barrel";
	}
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFloat bulletVolleySpread = FScalableFloat(0);
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFloat spreadMultiplier= FScalableFloat(1);

	/* Should be added to the bullet. */ /*
	// When Damage falloff begins to take affect. (If this is below 0 then there is no falloff.)
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFloat falloffBeginDistance = FScalableFloat(-1.f);
	// Distance where the falloff ends. (End distance is BeginDistance + Duration)
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFloat falloffDuration = FScalableFloat(0.f);
	// CurveTable used to alter damage for falloff. 0 = BeginDistance, 1 = BeginDistance + Duration
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	UCurveFloat* falloffCurve;
	*/
	// Niagara system when firing.
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	class UNiagaraSystem* MuzzleFlash;
	// Audio played when firing.
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	USoundBase* DefaultFiringSound;
	// For AI Noise
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFloat NoiseAmount = FScalableFloat(1);
	/* Created function so randomization or logic can be added when grabbing sound. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	USoundBase* GetFiringSound() const;
	USoundBase* GetFiringSound_Implementation() const { return DefaultFiringSound; }

	UFUNCTION(BlueprintCallable, Category = "FirearmData")
	float GetSpread(float volleyCount, float scale) const {
		return bulletVolleySpread.GetValueAtLevel(volleyCount) * spreadMultiplier.GetValueAtLevel(scale);
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
	UGunGripData() { 
		AttachSocketName = "Grip"; 
	}
	// Multiplies the default cam shake. The base value is found in the stock component.
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	FScalableFloat recoilMultiplier;
	// The base haptic feedback class
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	TObjectPtr<UForceFeedbackEffect> HapticFeedback;
	// Changes the intensity of the haptic feedback based on firearm level.
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	FScalableFloat HapticIntensity;
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	FScalableFloat CamShakeIntensity;
};
#pragma endregion
#pragma region Magazine
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunMagazineData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunMagazineData() { 
		AttachSocketName = "Magazine";
	}
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TArray<TSubclassOf<AActor>> BulletClasses;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	FScalableFloat MaxAmmo = FScalableFloat(30);
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	FScalableFloat ReloadSpeedMultiplier = FScalableFloat(1);
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	UAnimMontage* DefaultReloadMontage;
	/* Created function so randomization or logic can be added when grabbing montage. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	UAnimMontage* GetReloadMontage() const;
	UAnimMontage* GetReloadMontage_Implementation() const { return DefaultReloadMontage; }


};
#pragma endregion
#pragma region Sight
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunSightData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunSightData() { 
		AttachSocketName = "Sight";
	}
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	FScalableFloat FOVZoomMultiplier;
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	FScalableFloat FOVZoomAmount;
};
#pragma endregion
#pragma region Stock
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunStockData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UGunStockData() { 
		AttachSocketName = "Stock";
	}
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	UCurveLinearColor* BaseRecoil;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	FScalableFloat RecoilMultiplier = FScalableFloat(1);
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	FScalableFloat SwapMultiplier = FScalableFloat(1);
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	TSubclassOf<UCameraShakeBase> CamShake;
};
#pragma endregion

