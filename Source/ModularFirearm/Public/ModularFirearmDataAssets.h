// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataRegistryId.h"
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
USTRUCT(BlueprintType)
struct FScalableFirearmFloat {
	GENERATED_BODY()
public:
	FScalableFirearmFloat() :
		DefaultValue(1.f),
		Curve(FCurveTableRowHandle())
	{}
	FScalableFirearmFloat(float defaultValue) :
		DefaultValue(defaultValue),
		Curve(FCurveTableRowHandle()) 
	{}
	
	float GetValue() const {
		if (!IsValid(Curve.CurveTable)) {
			return DefaultValue;
		}
		return Curve.Eval(DefaultValue, RegistryType.GetName().ToString());
	}
	float GetValue(float scale) const {
		if (!IsValid(Curve.CurveTable)) {
			return DefaultValue;
		}
		return Curve.Eval(scale, RegistryType.GetName().ToString());
	}
/** Default value that is returned if a curve is not present. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScalableFirearmFloat")
	float DefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScalableFirearmFloat")
	FCurveTableRowHandle Curve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScalableFirearmFloat")
	FDataRegistryType RegistryType;
};

USTRUCT(BlueprintType)
struct FScalableFirearmSpread{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm")
	float Multiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm")
	UCurveFloat* Curve;
	FScalableFirearmSpread() :
		Multiplier(1.f),
		Curve(nullptr) {
	}
	FScalableFirearmSpread(float multiplier) :
		Multiplier(multiplier),
		Curve(nullptr) {
	}
	FScalableFirearmSpread(float multiplier, UCurveFloat* curve) :
		Multiplier(multiplier),
		Curve(curve) {
	}
	float GetValue(float modifier, float ) const {
		if (IsValid(Curve)) {
			return (Curve->GetFloatValue(modifier) * Multiplier);
		}
		return Multiplier;
	}
};


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
	FScalableFirearmFloat bulletVolleySpread = FScalableFirearmFloat(0);
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat spreadMultiplier = FScalableFirearmFloat(1.0);

	/* Should be added to the bullet. */ /*
	// When Damage falloff begins to take affect. (If this is below 0 then there is no falloff.)
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat falloffBeginDistance = FScalableFirearmFloat(-1.f);
	// Distance where the falloff ends. (End distance is BeginDistance + Duration)
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat falloffDuration = FScalableFirearmFloat(0.f);
	// Curve used to alter damage for falloff. 0 = BeginDistance, 1 = BeginDistance + Duration
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
	FScalableFirearmFloat NoiseAmount = FScalableFirearmFloat(0.f);
	/* Created function so randomization or logic can be added when grabbing sound. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm Data")
	USoundBase* GetFiringSound() const;
	USoundBase* GetFiringSound_Implementation() const { return DefaultFiringSound; }

	UFUNCTION(BlueprintCallable, Category = "FirearmData")
	float GetSpread(float volleyCount, float scale) const {
		return bulletVolleySpread.GetValue(volleyCount) * spreadMultiplier.GetValue(scale);
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
	FScalableFirearmFloat recoilMultiplier;
	// The base haptic feedback class
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	TObjectPtr<UForceFeedbackEffect> HapticFeedback;
	// Changes the intensity of the haptic feedback based on firearm level.
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	FScalableFirearmFloat HapticIntensity;
	UPROPERTY(EditAnywhere, Category = "Grip", meta = (DisplayPriority = 2))
	FScalableFirearmFloat CamShakeIntensity;
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
	FScalableFirearmFloat MaxAmmo;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	FScalableFirearmFloat ReloadSpeedMultiplier;
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
	FScalableFirearmFloat FOVZoomMultiplier;
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	FScalableFirearmFloat FOVZoomAmount;
	UPROPERTY(EditAnywhere, Category = "Sight", meta = (DisplayPriority = 2))
	FVector CameraAimOffset = FVector(0, 0, 0);
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
	FScalableFirearmFloat recoilMultiplierDuration;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	UCurveLinearColor* BaseRecoil;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	FScalableFirearmFloat RecoilMultiplier;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	FScalableFirearmFloat SwapMultiplier;
	UPROPERTY(EditAnywhere, Category = "Stock", meta = (DisplayPriority = 2))
	TSubclassOf<UCameraShakeBase> CamShake;
};
#pragma endregion