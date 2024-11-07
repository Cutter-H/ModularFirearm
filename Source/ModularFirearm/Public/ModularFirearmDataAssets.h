// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ModularFirearmDataAssets.generated.h"

UENUM(BlueprintType)
enum EFirearmComponentType : uint8{
	Receiver	=	0,
	Attachment	=	1,
	Barrel		=	2,
	Grip		=	3,
	Magazine	=	4,
	Sight		=	5,
	Stock		=	6,
	Num			=	7	UMETA(Hidden)
};

UENUM(BlueprintType)
enum EFiringType {
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
	float Multiplier;
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
	float GetValue(float modifier) {
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
	UPROPERTY(EditAnywhere, Category = "Info")
	FString FirearmName = "Firearm";
	UPROPERTY(EditAnywhere, Category = "Info")
	FString FirearmDescription = "A gun.";
	UPROPERTY(EditAnywhere, Category = "Info")
	UMaterialInstance* Icon;
	UPROPERTY()
	FString DefaultSkin;
	UPROPERTY(EditAnywhere, Category = "Stats")
	FScalableFirearmFloat GunDamage = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	FScalableFirearmFloat RoundsPerSecond = 5.f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	bool bAutomatic = true;
	UPROPERTY(EditAnywhere, Category = "Stats")
	TSubclassOf<AActor> BulletClass;

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
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UGunAttachmentData> Attachment;
};
#pragma endregion

#pragma region Component Base
UCLASS(NotBlueprintable, HideDropdown)
class MODULARFIREARM_API UGunPartDataBase : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName ComponentName = "Gun Component";
	UPROPERTY(EditAnywhere)
	FText ComponentDescription = FText::FromString("Lorem ipsum");
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> Mesh;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimInstance> DefaultAnimInstance;
	UPROPERTY(EditAnywhere)
	TMap<FString, UMaterialInterface*> Skins;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> Icon;
};

#pragma endregion
#pragma region Attachment
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunAttachmentData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
		// This is basically FlashLights and lasers.
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat LightIntensity;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraBeamSystem;
};
#pragma endregion
#pragma region Barrel
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunBarrelData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat bulletSpreadDegree = FScalableFirearmFloat(0);

	// When Damage falloff begins to take affect. (If this is below 0 then there is no falloff.)
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat falloffBeginDistance = FScalableFirearmFloat(-1.f);
	// Distance where the falloff ends. (End distance is BeginDistance + Duration)
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat falloffDuration = FScalableFirearmFloat(0.f);
	// Curve used to alter damage for falloff. 0 = BeginDistance, 1 = BeginDistance + Duration
	UPROPERTY(EditAnywhere)
		UCurveFloat* falloffCurve;
	// Niagara system when firing.
	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* MuzzleFlash;
	// Cascade system when firing.
	UPROPERTY(EditAnywhere)
		class UParticleSystem* MuzzleFlash_Cascade;
	// Audio played when firing.
	UPROPERTY(EditAnywhere)
		USoundBase* FiringSound;
	// For AI Noise
	UPROPERTY(EditAnywhere)
		float NoiseAmount = 0.f;


};
#pragma endregion
#pragma region Grip
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunGripData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	FScalableFirearmFloat recoilMultiplier;

};
#pragma endregion
#pragma region Magazine
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunMagazineData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat MaxAmmo;
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat ReloadSpeedMultiplier;
};
#pragma endregion
#pragma region Sight
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunSightData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat FOVZoomMultiplier;
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat FOVZoomAmount;
	UPROPERTY(EditAnywhere)
	FVector CameraAimOffset = FVector(0, 0, 0);
};
#pragma endregion
#pragma region Stock
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunStockData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat recoilMultiplierDuration;
	UPROPERTY(EditAnywhere)
	UCurveLinearColor* BaseRecoil;
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat RecoilMultiplier;
	UPROPERTY(EditAnywhere)
	FScalableFirearmFloat swapMultiplier;
};
#pragma endregion