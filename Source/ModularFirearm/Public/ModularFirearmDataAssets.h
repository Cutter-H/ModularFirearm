// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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

USTRUCT(BlueprintType)
struct FScalableFirearmFloat {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm")
	float Multiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm")
	UCurveFloat* Curve;
	FScalableFirearmFloat() :
		Multiplier(1.f),
		Curve(nullptr)
	{}
	FScalableFirearmFloat(float multiplier) :
		Multiplier(multiplier),
		Curve(nullptr) 
	{}
	FScalableFirearmFloat(float multiplier, UCurveFloat* curve) :
		Multiplier(multiplier),
		Curve(curve) 
	{}
	float GetValue(float modifier) const {
		if (IsValid(Curve)) {
			return (Curve->GetFloatValue(modifier) * Multiplier);
		}
		return Multiplier;
	}
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
	TMap<FString, UMaterialInterface*> Skins;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FName AttachSocketName = "Attachment";

};

#pragma endregion
#pragma region Attachment
UCLASS(meta = (PrioritizeCategories = "Gun Part"))
class MODULARFIREARM_API UGunAttachmentData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	// This is basically FlashLights and lasers.
	UPROPERTY(EditAnywhere, Category = "Attachment", meta = (DisplayPriority = 2))
	FScalableFirearmFloat LightIntensity;
	UPROPERTY(EditAnywhere, Category = "Attachment", meta = (DisplayPriority = 2))
	class UNiagaraSystem* NiagaraBeamSystem;
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
	FScalableFirearmFloat bulletSpreadDegree = FScalableFirearmFloat(0);

	// When Damage falloff begins to take affect. (If this is below 0 then there is no falloff.)
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat falloffBeginDistance = FScalableFirearmFloat(-1.f);
	// Distance where the falloff ends. (End distance is BeginDistance + Duration)
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat falloffDuration = FScalableFirearmFloat(0.f);
	// Curve used to alter damage for falloff. 0 = BeginDistance, 1 = BeginDistance + Duration
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	UCurveFloat* falloffCurve;
	// Niagara system when firing.
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	class UNiagaraSystem* MuzzleFlash;
	// Audio played when firing.
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	USoundBase* DefaultFiringSound;
	// For AI Noise
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat NoiseAmount = FScalableFirearmFloat(0.f);

	UFUNCTION(BlueprintNativeEvent, Category = "Barrel")
	USoundBase* GetFiringSound() const;
	USoundBase* GetFiringSound_Implementation() const { return DefaultFiringSound; }

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
	
	UFUNCTION(BlueprintNativeEvent, Category = "Magazine")
	UAnimMontage* GetReloadMontage() const;
	UAnimMontage* GetReloadMontage_Implementation() const {
		return DefaultReloadMontage; }


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