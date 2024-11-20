// Cutter Hodnett // 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularFirearmDataAssets.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangeSignature, int, newAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletSpawnSignature, AActor*, newBullet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimationSignature, UAnimMontage*, montage);

class UNiagaraSystem;
class UModularFirearmAttributeSet;
class UAbilitySystemComponent;

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
	void SetComponent(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent, int level = 1);

	UFUNCTION(BlueprintCallable, Category = "Firearm|Attachment")
	void SetComponentSkin(const EFirearmComponentType& componentType, const FString& skinName);

	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	void StartFiring();
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	void StartReloading();
	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	void StopReloading();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Firearm|Reloading")
	void LoadNewMagazine(bool bFreeFill = false);
#pragma endregion
#pragma region Getters/Setters
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetCurrentAmmo() const { return CurrentMagazineAmmo + bBulletChambered; }
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetMaxAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetBulletSpread(int volleyCount) const;
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
	float GetMultishot() const; 
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

	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	UNiagaraSystem* GetMuzzleFlash() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	USoundBase* GetFiringSound() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	UAnimMontage* GetReloadMontage();



#pragma endregion
#pragma region GAS
	UFUNCTION(BlueprintCallable, Category = "Firearm|GAS")
	FActiveGameplayEffectHandle ApplyGameplayEffectToFirearm(TSubclassOf<UGameplayEffect> gameplayEffectClass, int level, const FGameplayEffectContextHandle& effectContext = FGameplayEffectContextHandle());
	UFUNCTION(BlueprintCallable, Category = "Firearm|GAS")
	FActiveGameplayEffectHandle ApplyGameplayEffectSpecToFirearm(const FGameplayEffectSpec& gameplayEffectSpec);
#pragma endregion
protected:
#pragma region Firearm Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm", meta = (ExposeOnSpawn = "true"))
	bool bStartWithWeaponLoaded = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm")
	bool bPlayMontagesFromExternalSource = false;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts")
	bool bUseSimpleFirearm = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<EFiringMode> FiringMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<ETargetingMode> TargetingMode;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing", meta=(EditConditionHides, EditCondition="TargetingMode==ETargetingMode::DirectionOfMuzzle"))
	FRotator MuzzleOffset;
	UPROPERTY(BlueprintReadOnly, Category = "Firearm|Firing")
	int VolleyBulletCount = 0;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	TEnumAsByte<ECollisionChannel> TargetingChannel = ECollisionChannel::ECC_Visibility;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	FName MuzzleSocketName = "Muzzle";
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	float DefaultMultishot = 0.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	float DefaultFireRate = 10.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing|Burst")
	float DefaultBurstSpeed = 20.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing|Burst")
	int DefaultBurstAmount = 3;
	UPROPERTY(EditAnywhere, Category = "Firearm|Reloading")
	bool bRecycleAmmoOnReload = true;	
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UAbilitySystemComponent> AbilitySystemClass;
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UModularFirearmAttributeSet> AttributeSetClass;


	UPROPERTY(Replicated, meta = (ArraySizeEnum = "EFirearmComponentType"))
	TArray<FString> ComponentSkins;

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UModularFirearmAttributeSet> AttributeSet;
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
#pragma endregion
#pragma region Component Defaults
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	float DefaultNoise = 1.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Grip", meta = (DisplayThumbnail = "false"))
	TObjectPtr<UForceFeedbackEffect> DefaultFiringHaptic;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Grip")
	float DefaultFiringHapticIntensity;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Magazine")
	float DefaultMaxAmmo = 30.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Magazine")
	TSubclassOf<AActor> DefaultBulletClass;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	UAnimMontage* DefaultReloadMontage;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Stock")
	TSubclassOf<UCameraShakeBase> DefaultCamShake;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Stock")
	float DefaultCamShakeIntensity = 1.f;	
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle", meta=(DisplayThumbnail="false"))
	TObjectPtr<UCurveFloat> DefaultVolleySpread;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	float DefaultSpreadMultiplier = 1.f;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	USoundBase* DefaultFiringSound;
	UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	UNiagaraSystem* DefaultMuzzleFlash;

#pragma endregion
#pragma region Cosmetics
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	FString DefaultSkin = "Normal";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TMap<FString, UMaterialInterface*> ReceiverSkins;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TObjectPtr<UAnimMontage> FiringMontage;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm|Cosmetics")
	UAnimMontage* GetFiringMontage() const;
	UAnimMontage* GetFiringMontage_Implementation() const { return FiringMontage; }

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
	FTimerHandle BurstTimer;
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
	void FireWeapon(bool force = false);
	UFUNCTION()
	void BurstFireWeapon(int burst = 1);
	
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ReceiverMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BarrelMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> GripMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MagazineMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SightMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StockMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MuzzleMesh;
	UPROPERTY()
	TArray<UMeshComponent*> PartMeshes;
#pragma endregion
#pragma region Component Data
	FActiveGameplayEffectHandle MuzzleEffect;
	FActiveGameplayEffectHandle BarrelEffect;
	FActiveGameplayEffectHandle GripEffect;
	FActiveGameplayEffectHandle MagazineEffect; 
	FActiveGameplayEffectHandle SightEffect;
	FActiveGameplayEffectHandle StockEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Muzzle)
	TObjectPtr<UGunBarrelData> Muzzle;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Barrel)
	TObjectPtr<UGunBarrelData> Barrel;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Grip)
	TObjectPtr<UGunGripData> Grip;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Magazine)
	TObjectPtr<UGunMagazineData> Magazine;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Sight)
	TObjectPtr<UGunSightData> Sight;
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Stock)
	TObjectPtr<UGunStockData> Stock;

	UFUNCTION()
	void OnRep_Muzzle();
	UFUNCTION()
	void OnRep_Barrel();
	UFUNCTION()
	void OnRep_Grip();
	UFUNCTION()
	void OnRep_Magazine();
	UFUNCTION()
	void OnRep_Sight();
	UFUNCTION()
	void OnRep_Stock();

	UGunPartDataBase* GetPartData(const EFirearmComponentType& componentType) const {
		switch (componentType) {
		case 1: return Barrel;
		case 2: return Grip;
		case 3: return Magazine;
		case 4: return Sight;
		case 5: return Stock;
		case 6: return Muzzle;
		default: return nullptr;
		}
	}
	FActiveGameplayEffectHandle& GetEffect(const EFirearmComponentType& componentType) {
		switch (componentType) {
		case 1: return BarrelEffect;
		case 2: return GripEffect;
		case 3: return MagazineEffect;
		case 4: return SightEffect;
		case 5: return StockEffect;
		case 6: return MuzzleEffect;
		default: return;
		}
	}
	bool SetPartBaseData(UGunPartDataBase* part) {
		if (part->IsA<UGunMuzzleData>())	{ Muzzle = Cast<UGunMuzzleData>(part);			return true; }
		if (part->IsA<UGunBarrelData>())	{ Barrel = Cast<UGunBarrelData>(part);			return true; }
		if (part->IsA<UGunGripData>())		{ Grip = Cast<UGunGripData>(part);				return true; }
		if (part->IsA<UGunMagazineData>())	{ Magazine = Cast<UGunMagazineData>(part);		return true; }
		if (part->IsA<UGunSightData>())		{ Sight = Cast<UGunSightData>(part);			return true; }
		if (part->IsA<UGunStockData>())		{ Stock = Cast<UGunStockData>(part);			return true; }
		return false;
	}
#pragma endregion
};
