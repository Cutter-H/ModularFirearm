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
	Num			=	6	UMETA(Hidden)
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

/**
 * Classes for each type of Gun Component Data Asset
 */
#pragma region Firearm Data
UCLASS()
class MODULARFIREARM_API UModularFirearmData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm")
	FString FirearmName = "Firearm";
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	TEnumAsByte<ECollisionChannel> TargetingChannel = ECollisionChannel::ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Firing")
	FName MuzzleSocketName = "Muzzle";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Firing")
	FScalableFirearmFloat MultiShot = FScalableFirearmFloat(0);
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	FScalableFirearmFloat RoundsPerSecond = FScalableFirearmFloat(5.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Firing|Burst")
	FScalableFirearmFloat BurstSpeed = FScalableFirearmFloat(8);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Firing|Burst")
	int BurstAmount = 3;



	UPROPERTY(EditAnywhere, Category = "Firearm|Reloading")
	bool bRecycleAmmoOnReload = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Customization")
	FString DefaultSkin = "Normal";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Customization")
	TMap<FString, UMaterialInterface*> Skins;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGunMagazineData> Magazine;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGunBarrelData> Barrel;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGunStockData> Stock;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGunSightData> Sight;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGunGripData> Grip;
	
	UGunPartDataBase* GetPartData(EFirearmComponentType type) const {
		switch (type) {
		case 1:
			return Barrel;
		case 2:
			return Grip;
		case 3:
			return Magazine;
		case 4:
			return Sight;
		case 5:
			return Stock;
		default:
			return nullptr;
		}
	}
};
#pragma endregion

#pragma region Component Base
UCLASS(NotBlueprintable, HideDropdown)
class MODULARFIREARM_API UGunPartDataBase : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FName ComponentName = "Gun Component";
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	FText ComponentDescription = FText::FromString("Lorem ipsum");
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TObjectPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TSubclassOf<UAnimInstance> DefaultAnimInstance;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TMap<FString, UMaterialInterface*> Skins;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TObjectPtr<UMaterialInterface> Icon;
	UPROPERTY(EditAnywhere, Category = "GunPart", meta = (DisplayPriority = 1))
	TEnumAsByte<EFirearmComponentType> AttachesTo;
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
		ComponentName = "Default Barrel";
	}
	UPROPERTY(EditAnywhere, Category = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat bulletSpreadDegree = FScalableFirearmFloat(0);

	// When Damage falloff begins to take affect. (If this is below 0 then there is no falloff.)
	UPROPERTY(EditAnywhereCategory = "Barrel", meta = (DisplayPriority = 2))
	FScalableFirearmFloat falloffBeginDistance = FScalableFirearmFloat(-1.f);
	// Distance where the falloff ends. (End distance is BeginDistance + Duration)
	UPROPERTY(EditAnywhereCategory = "Barrel", meta = (DisplayPriority = 2))
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

	UFUNCTION(BlueprintNativeEvent, Categpry = "Barrel")
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
		ComponentName = "Default Grip";
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
		ComponentName = "Default Magazine";
	}
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	TArray<TSubclassOf<AActor>> BulletClasses;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	FScalableFirearmFloat MaxAmmo;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	FScalableFirearmFloat ReloadSpeedMultiplier;
	UPROPERTY(EditAnywhere, Category = "Magazine", meta = (DisplayPriority = 2))
	UAnimMontage* DefaultReloadMontage;
	UFUNCTION(BlueprintNativeEvent, Categpry = "Magazine")
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
		ComponentName = "Default Sight";
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
		ComponentName = "Default Stock";
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