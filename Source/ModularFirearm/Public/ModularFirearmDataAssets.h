// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ModularFirearmDataAssets.generated.h"


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
	float GunDamage = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float RoundsPerSecond = 5.f;
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
	FName PartName = "Gun Component";
	UPROPERTY(EditAnywhere)
	FText FirearmDescription = FText::FromString("Lorem ipsum");
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> Mesh;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimInstance> DefaultAnimInstance;
	UPROPERTY(EditAnywhere)
	TMap<FString, UMaterialInterface*> AlternativeSkins;
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
		float LightIntensity = 1.f;
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
		float bulletSpreadDegree = 0.f;

	// When Damage falloff begins to take affect. (If this is below 0 then there is no falloff.)
	UPROPERTY(EditAnywhere)
		float falloffBeginDistance = -1.f;
	// Distance where the falloff ends. (End distance is BeginDistance + Duration)
	UPROPERTY(EditAnywhere)
		float falloffDuration = 0.f;
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
		float recoilMultiplier = 1.f;

};
#pragma endregion
#pragma region Magazine
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunMagazineData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		int MaxAmmo = 1;
	UPROPERTY(EditAnywhere)
		float ReloadSpeedMultiplier = 1.f;
};
#pragma endregion
#pragma region Sight
UCLASS(meta = (PrioritizeCategories = "GunDataAssetBase"))
class MODULARFIREARM_API UGunSightData : public UGunPartDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		float FOVZoomMultiplier = 1.f;
	UPROPERTY(EditAnywhere)
		float FOVZoomAmount = 0.f;
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
		float recoilMultiplierDuration = 1.f;
	UPROPERTY(EditAnywhere)
		UCurveLinearColor* RecoilMultiplier;
	UPROPERTY(EditAnywhere)
		float swapMultiplier = 1.f;
};
#pragma endregion