// Cutter Hodnett // 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularFirearmDataAssets.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangeSignature, int, newAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletSpawnSignature, AActor*, newBullet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimationSignature, UAnimMontage*, montage);

UCLASS(PrioritizeCategories = ("Firearm"))
class MODULARFIREARM_API AModularFirearm : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region Delegates
	UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnAmmoChangeSignature OnCurrentAmmoChange;
	UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnBulletSpawnSignature OnBulletSpawn;
	UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPlayAnimationSignature OnFiringMontagePlay;
	UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPlayAnimationSignature OnReloadMontagePlay;
	UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPlayAnimationSignature OnReloadMontageStop;
#pragma endregion
#pragma region Core Functions
	UFUNCTION(BlueprintCallable, Category = "Firearm|Attachment")
	void SetComponent(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent);

	UFUNCTION(BlueprintCallable, Category = "Firearm|Attachment")
	void SetComponentSkin(const EFirearmComponentType& componentType, const FString& skinName);

	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	void BeginFiring();
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	void StartReloading();
	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	void StopReloading();
#pragma endregion
#pragma region Getters/Setters
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetCurrentAmmo() const { return CurrentMagazineAmmo + bBulletChambered; }
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetMaxAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetBulletSpread() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetNoise() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	UForceFeedbackEffect* GetHapticFeedback() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetHapticIntensity() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	TSubclassOf<UCameraShakeBase> GetCamShake() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetCamShakeIntensity() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetFireRate() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetBurstSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetBurstAmount() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	FTransform GetMuzzleTransform() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	TSubclassOf<AActor> GetBulletClass() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetReloadSpeedModifier() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm|Getters")
	int GetReserveAmmo() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Firearm|Setters")
	void SetReserveAmmo(int newReserveAmmo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm|Getters")
	float GetScalingAttribute() const;
	float GetScalingAttribute_Implementation() const { return 1.f; }
#pragma endregion
protected:
#pragma region Firearm Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm", meta = (ExposeOnSpawn = "true"))
	bool bStartWithWeaponLoaded = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm")
	bool bPlayMontagesFromExternalSource = false;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts")
	bool bUseSimpleFirearm = false;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	TEnumAsByte<EFiringMode> FiringMode;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	TEnumAsByte<ETargetingMode> TargetingMode;
	UPROPERTY(Replicated, meta = (ArraySizeEnum = "EFirearmComponentType"))
	TArray<FString> ComponentSkins;
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
#pragma endregion
#pragma region Component Defaults
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel")
	FScalableFirearmFloat DefaultBulletSpread = FScalableFirearmFloat(0);
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel")
	FScalableFirearmFloat DefaultNoise = FScalableFirearmFloat(0);
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Grip")
	TObjectPtr<UForceFeedbackEffect> DefaultFiringHaptic;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Grip")
	float DefaultFiringHapticIntensity;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Magazine")
	FScalableFirearmFloat DefaultMaxAmmo = FScalableFirearmFloat(30);
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Magazine")
	TSubclassOf<AActor> DefaultBulletClass;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Stock")
	TSubclassOf<UCameraShakeBase> DefaultCamShake;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Stock")
	float DefaultCamShakeIntensity = 1.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel")
	TObjectPtr<UCurveFloat> BulletSpread;
#pragma endregion
#pragma region Cosmetics
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	FString DefaultSkin = "Normal";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TMap<FString, UMaterialInterface*> Skins;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TObjectPtr<UAnimMontage> FiringMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TObjectPtr<UAnimMontage> DefaultReloadMontage;

	UFUNCTION(NetMulticast, Reliable)
	void PlayReplicatedMontage(UAnimMontage* montage, const FString& info = "");
	UFUNCTION()
	void OnReceiverMontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion
#pragma region Parent Overrides
	AModularFirearm();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
#pragma endregion
private:
#pragma region Bullet Functionality
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentAmmo)
	bool bBulletChambered = false;
	UPROPERTY(Replicated)
	int CurrentMagazineAmmo = 0;
	UPROPERTY()
	FTimerHandle FiringTimer;
	UPROPERTY()
	FTimerHandle RecoilTimer;
	UPROPERTY()
	bool bWantsToFire = false;
	UPROPERTY()
	bool bReloading = false;

	UFUNCTION(Server, Reliable)
	void ReloadOnServer(bool start = true);
	UFUNCTION(Server, Reliable)
	void SpawnBullet(const FVector& targetLocation);
	UFUNCTION()
	void OnRep_CurrentAmmo();
	UFUNCTION()
	void FireWeapon(int burst = 1);
	UFUNCTION()
	void LoadNewMagazine(bool bFreeFill = false);
#pragma endregion
#pragma region Replication
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateSkinChange(const EFirearmComponentType& componentType, const FString& skinName);
	UFUNCTION(Server, Reliable)
	void SetComponentOnServer(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent);
	UFUNCTION(Server, Reliable)
	void SetComponentSkinOnServer(const EFirearmComponentType& componentType, const FString& skinName);
	UFUNCTION()
	void UpdateSkin(const EFirearmComponentType& componentType, const FString& skinName);
#pragma endregion
#pragma region Mesh Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ReceiverMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BarrelMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> GripMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MagazineMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SightMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StockMesh;

	TArray<UMeshComponent*> PartMeshes;
#pragma endregion
#pragma region Component Data

	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides), Replicated, ReplicatedUsing = OnRep_Barrel)
	TObjectPtr<UGunBarrelData> Barrel;
	UFUNCTION()
	void OnRep_Barrel();

	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides), Replicated, ReplicatedUsing = OnRep_Grip)
	TObjectPtr<UGunGripData> Grip;
	UFUNCTION()
	void OnRep_Grip();

	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides), Replicated, ReplicatedUsing = OnRep_Magazine)
	TObjectPtr<UGunMagazineData> Magazine;
	UFUNCTION()
	void OnRep_Magazine();

	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides), Replicated, ReplicatedUsing = OnRep_Sight)
	TObjectPtr<UGunSightData> Sight;
	UFUNCTION()
	void OnRep_Sight();

	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides), Replicated, ReplicatedUsing = OnRep_Stock)
	TObjectPtr<UGunStockData> Stock;
	UFUNCTION()
	void OnRep_Stock();
	UGunPartDataBase* GetPartData(const EFirearmComponentType& componentType) const {
		switch (componentType) {
		case 1: return Barrel;
		case 2: return Grip;
		case 3: return Magazine;
		case 4: return Sight;
		case 5: return Stock;
		default: return nullptr;
		}
	}
	bool SetPartBaseData(UGunPartDataBase* part) {
		if (part->IsA<UGunBarrelData>())	{ Barrel = Cast<UGunBarrelData>(part);			return true; }
		if (part->IsA<UGunGripData>())		{ Grip = Cast<UGunGripData>(part);				return true; }
		if (part->IsA<UGunMagazineData>())	{ Magazine = Cast<UGunMagazineData>(part);		return true; }
		if (part->IsA<UGunSightData>())		{ Sight = Cast<UGunSightData>(part);			return true; }
		if (part->IsA<UGunStockData>())		{ Stock = Cast<UGunStockData>(part);			return true; }
		return false;
	}
#pragma endregion
};
