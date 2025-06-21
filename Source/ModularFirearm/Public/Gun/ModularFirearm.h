// Cutter Hodnett // 2024-

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularFirearmInterface.h"
#include "ModularFirearmTypes.h"
#include "ModularFirearm.generated.h"

class UModularFirearmCustomizationComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGenericFirearmEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartChangedSignature, EFirearmComponentType, type, UGunPartDataBase*, newPart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartSkinChangedSignature, EFirearmComponentType, type, FString, newSkin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangeSignature, int, newAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletSpawnSignature, AActor*, newBullet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimationSignature, UAnimMontage*, montage);

class UNiagaraSystem;
class UGunPartDataBase;
class UGunGripData;
class UGunMagazineData;
class UGunMuzzleData;
class UGunSightData;
class UGunStockData;
class UGunBarrelData;

USTRUCT(BlueprintType)
struct FModularFirearmFiringData{
	GENERATED_BODY()
	FModularFirearmFiringData() {}
	/**
	 * The firing mode this weapon starts with.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EFiringMode> DefaultFiringMode = EFiringMode::MFFM_Automatic;
	/**
	 * Available firing modes this weapon can swap to.
	 * If only one is available, it's recommended to leave this empty to not allow swapping.
	 * The default firing mode will always be initially set.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EFiringMode>> AvailableFiringModes;
	/**
	* Increases the number of additional bullets used when firing the weapon. (Useful for shotguns)
	* Decimal digits determine chance for additional bullet. Example: 0.1 -> 10% change to fire 1 additional bullet.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Multishot = 0.f;
	/**
	* The number of bullets that can be fired per second.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FiringRate = 12.5f;
	/*
	* The number of bullets that are fired in the burst volley.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "FiringModes.Contains(EFiringMode::MFFM_Burst)", EditConditionHides))
	int BurstCount = 3;
	/**
	* The fire rate at which the burst volley is fired.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "FiringModes.Contains(EFiringMode::MFFM_Burst)", EditConditionHides))
	float BurstSpeed = 20.f;	
};

USTRUCT(BlueprintType)
struct FModularFirearmTargetingData {
	GENERATED_BODY()
	
	/*
	* The collision channel used for hit detection for the TargetingModes FocalPoint and CursorLocation.
	*/
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	TEnumAsByte<ECollisionChannel> TargetingChannel = ECollisionChannel::ECC_Visibility;
	
	/*
	* Changes the direction of the bullets when spawning.
	* FocalPoint:			Targets bullets toward the center of the player's screen or the AI's FocusPoint.
	* DirectionOfMuzzle:	Targets the bullet directly out of the muzzle. To accomodate, this is combined with the static value of MuzzleOffset.
	* CursorLocation:		Does a hit under the cursor and targets the bullet to that location.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<ETargetingMode> DefaultTargetingMode = ETargetingMode::MFTM_FocalPoint;
	/**
	 * Available TargetingMode this weapon can swap to.
	 * If only one is available, it's recommended to leave this empty to not allow swapping.
	 * The default targeting mode will always be initially set.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm|Firing")
	TArray<TEnumAsByte<ETargetingMode>> AvailableTargetingModes;
	
};

UCLASS(PrioritizeCategories = ("Firearm"))
class MODULARFIREARM_API AModularFirearm : public AActor, public IModularFirearmInterface
{
	GENERATED_BODY()
	
public:
	/*
	* Skin used by default.
	* Skins are applied to the 0 material slot.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	FString DefaultSkin = "Normal";

	UFUNCTION()
	UGunPartDataBase* GetPartData(EFirearmComponentType type);
#pragma region Delegates
	/*
	* Called when the weapon starts firing.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnGenericFirearmEventSignature OnBeginFiring;
	/*
	* Called when the weapon stops firing.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnGenericFirearmEventSignature OnEndFiring;
	/*
	* Called when the weapon starts reloading.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnGenericFirearmEventSignature OnBeginReloading;
	/*
	* Called when the weapon stops reloading.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnGenericFirearmEventSignature OnEndReloading;
	/*
	* Called on every firing event. First time after OnBeginFiring. 
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnGenericFirearmEventSignature OnFire;
	/*
	* Called when a part's skin is changed.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPartSkinChangedSignature OnSkinChanged;
	/*
	* Called when a part is changed.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPartChangedSignature OnPartChanged;
	/*
	* Called when Current Magazine Ammo or Chambered Bullet are altered.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnAmmoChangeSignature OnCurrentAmmoChange;
	/*
	* Called on the server when a bullet is spawned.
	* This is called for EVERY bullet of Multishot, so this may be called multiple times when firing.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnBulletSpawnSignature OnBulletSpawn;
#pragma endregion
#pragma region Interface Overrides

	UFUNCTION()
	virtual AModularFirearm* AsModularFirearm() override {return this;}
	UFUNCTION(Server, Reliable)
	virtual void SetModularPart(EFirearmComponentType componentType, UGunPartDataBase* part) override;
	UFUNCTION(Server, Reliable)
	virtual void SetModularPartSkin(EFirearmComponentType componentType, const FString& skinName) override;
	UFUNCTION(Server, Reliable)
	virtual void SetAllSkins(const FString& skinName) override;
	UFUNCTION()
	virtual void StartFiring() override;
	UFUNCTION()
	virtual void StopFiring() override;
	UFUNCTION()
	virtual void Reload() override;
	UFUNCTION()
	virtual void StopReloading() override;
	UFUNCTION()
	virtual void TryChangeTargetingMode(ETargetingMode newMode, bool force = false) override;
	UFUNCTION()
	virtual void TryChangeFiringMode(EFiringMode newMode, bool bForce) override;
	UFUNCTION()
	virtual void TryIncrementFiringMode() override;
	UFUNCTION()
	virtual EFiringMode GetFiringMode() const override { return FiringMode; }
	UFUNCTION()
	virtual ETargetingMode GetTargetingMode() const override { return TargetingMode; }
	UFUNCTION()
	virtual void LoadNewMagazine(bool bFreeFill = false) override;
	UFUNCTION()
	virtual FModularFirearmAmmo GetAmmo() const override;
	UFUNCTION()
	virtual UGunPartDataBase* GetPart(EFirearmComponentType type) const override;
	UFUNCTION()
	virtual UGunBarrelData* GetBarrel() const override {return Barrel;}
	UFUNCTION()
	virtual UGunMuzzleData* GetMuzzle() const override {return Muzzle;}
	UFUNCTION()
	virtual UGunGripData* GetGrip() const override {return Grip;}
	UFUNCTION()
	virtual UGunMagazineData* GetMagazine() const override {return Magazine;}
	UFUNCTION()
	virtual UGunSightData* GetSight() const override {return Sight;}
	UFUNCTION()
	virtual UGunStockData* GetStock() const override {return Stock;}
	
	
#pragma endregion
#pragma region Getters/Setters
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	virtual int GetCurrentAmmo() const { return CurrentMagazineAmmo + bBulletChambered; }
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	int GetMaxAmmo() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetBulletSpread(int volleyCount) const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetNoise() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	UForceFeedbackEffect* GetHapticFeedback() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetHapticIntensity() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	TSubclassOf<UCameraShakeBase> GetCamShake() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetCamShakeIntensity() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetFireRate() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetMultishot() const; 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetBurstSpeed() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	int GetBurstAmount() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	FTransform GetMuzzleTransform() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	TSubclassOf<AActor> GetBulletClass(int bulletType = 0) const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	float GetReloadSpeedModifier() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	int GetReserveAmmo() const;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Firearm|Setters")
	void SetReserveAmmo(int newReserveAmmo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	TSoftObjectPtr<UNiagaraSystem> GetMuzzleFlash() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	TSoftObjectPtr<USoundBase> GetFiringSound() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Firearm|Getters")
	UAnimMontage* GetReloadMontage();



#pragma endregion
	
protected:
#pragma region Firearm Variables
	/*
	* Set to true when Start Firing is called. Set to false when stop firing is called.
	*/UPROPERTY(ReplicatedUsing=OnRep_IsFiring)
	bool bIsFiring = false;
	/*
	* Set to true when Reload is called. Set to false when the reload is stopped using the OnReceiverMontageEnded function.
	*/UPROPERTY(ReplicatedUsing=OnRep_IsReloading)
	bool bIsReloading = false;
	/*
	* If true, the weapon will set current ammo to max ammo at Beginplay. Afterwards this is set to false.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm", meta = (ExposeOnSpawn = "true"))
	bool bStartWithWeaponLoaded = true;
	/*
	* If true, a chambered round will be available but not exist in the Magazine. 
	* For example, reloading a non-empty 30 round magazine will result in 31 current ammo.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm", meta = (ExposeOnSpawn = "true"))
	bool bUsesChamberedRounds = true;
	/*
	*  If true, you cannot modify this weapon with parts. Only with the receiver mesh and this actor.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Parts")
	bool bUseSimpleFirearm = false;
	/*
	* Firing mode for the weapon. This is not replicated as firing is done locally then the bullet is spawned on the server. Afterwards, the effects are replicated.
	* Automatic:		The weapon will continue firing while until exhausted or StopFiring is called.
	* SemiAutomatic:	The weapon will fire once per StartFiring call.
	* Burst:			The weapon will fire a set number of times in quick succession once per StartFiring call.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<EFiringMode> FiringMode;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	FModularFirearmFiringData FiringData;
	/*
	* Changes the direction of the bullets when spawning.
	* FocalPoint:			Targets bullets toward the center of the player's screen or the AI's FocusPoint.
	* DirectionOfMuzzle:	Targets the bullet directly out of the muzzle. To accomodate, this is combined with the static value of MuzzleOffset.
	* CursorLocation:		Does a hit under the cursor and targets the bullet to that location.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<ETargetingMode> TargetingMode;
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	FModularFirearmTargetingData TargetingData;
	/*
	* Utilized when using DirectionOfMuzzle for TargetingMode. This rotation is combined with the rotation of the muzzle.
	*/
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing", meta = (EditConditionHides, EditCondition = "TargetingMode==ETargetingMode::DirectionOfMuzzle"))
	FRotator MuzzleOffset = FRotator(0, 90, 0);
	/*
	* How many bullets have been fired in the current volley. 
	* This is iterated after the bullet is fired.
	* Starts at 0.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Firearm|Firing")
	int VolleyBulletCount = 0;
	/*
	* This is the socket name for where bullets will be spawned.
	* Socket is checked on the meshes in this order:
	* 1: Muzzle Attachment
	* 2: Barrel Attachment
	* 3: Receiver
	*/
	UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	FName BulletSpawningSocket = "Muzzle";
	
	/*
	* If true, only missing ammo will be taken from reserves.
	* If false, an entire magazine will always be taken from reserves.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Reloading")
	bool bRecycleAmmoOnReload = true;
#pragma endregion
#pragma region Component Defaults
	/*
	* Dictates noise on weapon firing. This is commonly used in AI. Note that this is NOT SFX volume.
	* This value is overridden by Barrel and/or Muzzle parts.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	float DefaultNoise = 1.f;
	/*
	* The Feedback or Controller Rumble used when firing.
	* This value is overridden by Grip parts.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Grip", meta = (DisplayThumbnail = "false"))
	TObjectPtr<UForceFeedbackEffect> DefaultFiringHaptic;
	/*
	* Affects the scale of haptic feedback in controllers when firing the gun.
	* This value is overridden by Grip parts.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Grip")
	float DefaultFiringHapticIntensity;
	/*
	* Changes the max amount of bullets per magazine.
	* This value is overridden by Magazine parts.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Magazine")
	float DefaultMaxAmmo = 30.f;
	/*
	* The bullet class that is fired.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Magazine")
	TSubclassOf<AActor> DefaultBulletClass;
	/*
	* Montage to be played when reloading.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	UAnimMontage* DefaultReloadMontage;
	/*
	* Camera Shake that is played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Stock")
	TSubclassOf<UCameraShakeBase> DefaultCamShake;
	/*
	* The scale of the firing Camera Shake.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Stock")
	float DefaultCamShakeIntensity = 1.f;	
	/*
	* Curve used when firing volleys. Time is incremented per bullet. If this doesn't exist there will be no spread.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle", meta = (DisplayThumbnail = "false"))
	TObjectPtr<UCurveFloat> DefaultVolleySpread;
	/*
	* Multiplier for the VolleySpread.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	float SpreadMultiplier = 1.f;
	/*
	* 3D sound played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	TSoftObjectPtr<USoundBase> DefaultFiringSound;
	/*
	* Niagara effect played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	TSoftObjectPtr<UNiagaraSystem> DefaultMuzzleFlash;
#pragma endregion
#pragma region Animation
	
	/*
	* Skins available to the receiver mesh.
	* Skins are applied to the 0 material slot.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TMap<FString, UMaterialInterface*> ReceiverSkins;
	/*
	* Montage played when firing.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	TObjectPtr<UAnimMontage> FiringMontage;
	/*
	* Created to easily override and add logic when grabbing the firing montage.
	*/UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Firearm|Cosmetics")
	UAnimMontage* GetFiringMontage() const;
	UAnimMontage* GetFiringMontage_Implementation() const { return FiringMontage; }

	/*
	* Used to replicate montages on the receiver mesh.
	*/UFUNCTION(NetMulticast, Reliable)
	void PlayReplicatedMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Reliable)
	void StopReplicatedMontage(UAnimMontage* montage);	
	/*
	* Bound to the receiver mesh's anim instance. Called when ANY montage ends.
	*/UFUNCTION()
	void OnReceiverMontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion
#pragma region Parent Overrides
	AModularFirearm();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void PostLoad() override;
#pragma endregion
private:
#pragma region Bullet Functionality
	/*
	* Is a bullet currently chambered. This bullet exists outside of the Magazine.
	*/UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentAmmo)
	bool bBulletChambered = false;
	/*
	* Number of bullets left in the magazine.
	*/UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentAmmo)
	int CurrentMagazineAmmo = 0;
	/*
	* Timer used for firing. This is normally bound to the FireWeapon function.
	*/UPROPERTY()
	FTimerHandle FiringTimer;
	/*
	* Timer is not normally bound to a function. When this is active, the weapon cannot fire.
	*/UPROPERTY()
	FTimerHandle RecoilTimer;
	/*
	* This is changed to true when StartFiring is called and changed to false when StopFiring is called.
	*/UPROPERTY()
	bool bWantsToFire = false;
	/*
	* Spawns the bullet on the server for replication.
	*/UFUNCTION(Server, Reliable)
	void SpawnBullet(const FVector& targetDirection);
	/*
	* Called when bBulletChambered or CurrentMagazineAmmo are changed.
	*/UFUNCTION()
	void OnRep_CurrentAmmo();
	/*
	* Main worker function for firing. This is only used on the client firing.
	*/UFUNCTION()
	void FireWeapon(bool force = false);
	/*
	* Worker function that wraps FireWeapon when using the FiringMode Burst.
	*/UFUNCTION()
	void BurstFireWeapon(int burst = 1);
	
#pragma endregion
#pragma region Replication
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateSkinChange(const EFirearmComponentType& componentType, const FString& skinName);
#pragma endregion
#pragma region Mesh Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ReceiverMesh;
	UPROPERTY(BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BarrelMesh;
	UPROPERTY(BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> GripMesh;
	UPROPERTY(BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MagazineMesh;
	UPROPERTY(BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SightMesh;
	UPROPERTY(BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StockMesh;
	UPROPERTY(BlueprintReadOnly,  Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MuzzleMesh;
	UPROPERTY()
	TArray<UMeshComponent*> PartMeshes;
	UPROPERTY()
	TObjectPtr<UModularFirearmCustomizationComponent> CustomizationComponent;
#pragma endregion
#pragma region Component Data
	UPROPERTY(EditAnywhere, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleFirearm", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Muzzle)
	TObjectPtr<UGunMuzzleData> Muzzle;
	UPROPERTY(EditAnywhere, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleFirearm", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Barrel)
	TObjectPtr<UGunBarrelData> Barrel;
	UPROPERTY(EditAnywhere, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleFirearm", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Grip)
	TObjectPtr<UGunGripData> Grip;
	UPROPERTY(EditAnywhere, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleFirearm", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Magazine)
	TObjectPtr<UGunMagazineData> Magazine;
	UPROPERTY(EditAnywhere, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleFirearm", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Sight)
	TObjectPtr<UGunSightData> Sight;
	UPROPERTY(EditAnywhere, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleFirearm", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Stock)
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
	UFUNCTION()
	void OnRep_IsFiring();
	UFUNCTION()
	void OnRep_IsReloading();
#pragma endregion
};





