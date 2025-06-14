// Cutter Hodnett // 2024-

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "ModularFirearmDataAssets.h"
#include "ModularFirearmInterface.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangeSignature, int, newAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletSpawnSignature, AActor*, newBullet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimationSignature, UAnimMontage*, montage);

class UNiagaraSystem;
class UModularFirearmAttributeSet;
class UAbilitySystemComponent;

UCLASS(PrioritizeCategories = ("Firearm"))
class MODULARFIREARM_API AModularFirearm : public AActor, public IModularFirearmInterface
{
	GENERATED_BODY()
	
public:	
#pragma region Delegates
	/*
	* Called when Current Magazine Ammo or Chambered Bullet are altered.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnAmmoChangeSignature OnCurrentAmmoChange;
	/*
	* Called on the server when a bullet is spawned.
	* This is called for EVERY bullet of Multishot, so this may be called multiple times when firing.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnBulletSpawnSignature OnBulletSpawn;
	/*
	* Called on all instances when the firing montage should play.
	* If PlayMontagesFromExternalSource is enabled. This is called, but no montage is played.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPlayAnimationSignature OnFiringMontagePlay;
	/*
	* Called on all instances when the reload montage should play.
	* If PlayMontagesFromExternalSource is enabled. This is called, but no montage is played.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPlayAnimationSignature OnReloadMontagePlay;
	/*
	* Called on all instances when the Reload montage stops.
	* This is independent from PlayMontagesFromExternalSource. This is called whenever the montage is played that matches ReloadMontage.
	*/UPROPERTY(BlueprintAssignable, Category = "Firearm")
	FOnPlayAnimationSignature OnReloadMontageStop;
#pragma endregion
#pragma region Interface Overrides

	UFUNCTION()
	virtual AModularFirearm* AsModularFirearm() override {return this;}
	UFUNCTION()
	virtual bool SetModularPart(EFirearmComponentType componentType, UGunPartDataBase* part, int level = 1) override;
	UFUNCTION()
	virtual bool SetModularPartSkin(EFirearmComponentType componentType, const FString& skinName) override;
	UFUNCTION()
	virtual void StartFiring() override;
	UFUNCTION()
	virtual void StopFiring() override;
	UFUNCTION()
	virtual void Reload() override;
	UFUNCTION()
	virtual void StopReloading() override;
	UFUNCTION()
	virtual void LoadNewMagazine(bool bFreeFill = false) override;
	UFUNCTION()
	virtual FModularFirearmAmmo GetAmmo() const override;
	UFUNCTION()
	virtual float GetAttribute(FGameplayAttribute attribute) const override;
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
	TSoftObjectPtr<UAnimMontage> GetReloadMontage();



#pragma endregion
#pragma region GAS
	UFUNCTION(BlueprintCallable, Category = "Firearm|GAS")
	FActiveGameplayEffectHandle ApplyGameplayEffectToFirearm(TSubclassOf<UGameplayEffect> gameplayEffectClass, int level, const FGameplayEffectContextHandle& effectContext = FGameplayEffectContextHandle());
	UFUNCTION(BlueprintCallable, Category = "Firearm|GAS")
	FActiveGameplayEffectHandle ApplyGameplayEffectSpecToFirearm(const FGameplayEffectSpec& gameplayEffectSpec);
#pragma endregion
protected:
#pragma region Firearm Variables
	/*
	* If true, the weapon will set current ammo to max ammo at Beginplay. Afterwards this is set to false.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm", meta = (ExposeOnSpawn = "true"))
	bool bStartWithWeaponLoaded = true;
	/*
	* If true, a chambered round will be available but not exist in the Magazine. 
	* For example, reloading a non-empty 30 round magazine will result in 31 current ammo.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm", meta = (ExposeOnSpawn = "true"))
	bool bUsesChamberedRounds = true;
	/*
	* If true, the weapon won't fire any play montage's on the receiver, but will still call the montage Dispatchers.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm")
	bool bPlayMontagesFromExternalSource = false;
	/*
	*  If true, you cannot modify this weapon with parts. Only with the receiver mesh and this actor.
	*/UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts")
	bool bUseSimpleFirearm = false;
	/*
	* Firing mode for the weapon. This is not replicated as firing is done locally then the bullet is spawned on the server. Afterwards, the effects are replicated.
	* Automatic:		The weapon will continue firing while until exhausted or StopFiring is called.
	* SemiAutomatic:	The weapon will fire once per StartFiring call.
	* Burst:			The weapon will fire a set number of times in quick succession once per StartFiring call.
	*/UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<EFiringMode> FiringMode;
	/*
	* Changes the direction of the bullets when spawning.
	* FocalPoint:			Targets bullets toward the center of the player's screen or the AI's FocusPoint.
	* DirectionOfMuzzle:	Targets the bullet directly out of the muzzle. To accomodate, this is combined with the static value of MuzzleOffset.
	* CursorLocation:		Does a hit under the cursor and targets the bullet to that location.
	*/UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firearm|Firing")
	TEnumAsByte<ETargetingMode> TargetingMode;
	/*
	* Utilized when using DirectionOfMuzzle for TargetingMode. This rotation is combined with the rotation of the muzzle.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing", meta = (EditConditionHides, EditCondition = "TargetingMode==ETargetingMode::DirectionOfMuzzle"))
	FRotator MuzzleOffset;
	/*
	* How many bullets have been fired in the current volley. 
	* This is iterated after the bullet is fired.
	* Starts at 0.
	*/UPROPERTY(BlueprintReadOnly, Category = "Firearm|Firing")
	int VolleyBulletCount = 0;
	/*
	* The collision channel used for hit detection for the TargetingModes FocalPoint and CursorLocation.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	TEnumAsByte<ECollisionChannel> TargetingChannel = ECollisionChannel::ECC_Visibility;
	/*
	* This is the socket name for where bullets will be spawned.
	* Socket is checked on the meshes in this order:
	* 1: Muzzle Attachment
	* 2: Barrel Attachment
	* 3: Receiver
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	FName BulletSpawningSocket = "Muzzle";
	/*
	* Increases the number of additional bullets used when firing the weapon. (Useful for shotguns)
	* Decimal digits determine chance for additiona bullet. Example: 0.1 -> 10% change to fire 1 additional bullet.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	float DefaultMultishot = 0.f;
	/*
	* The number of bullets that can be fired per second.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing")
	float DefaultFireRate = 10.f;
	/*
	* The fire rate at which the burst volley is fired.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing|Burst")
	float DefaultBurstSpeed = 20.f;
	/*
	* The number of bullets that are fired in the burst volley.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Firing|Burst")
	int DefaultBurstAmount = 3;
	/*
	* If true, only missing ammo will be taken from reserves.
	* If false, an entire magazine will always be taken from reserves.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|Reloading")
	bool bRecycleAmmoOnReload = true;	
	/*
	* The class used for the firearm's Ability System Component.
	*/UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UAbilitySystemComponent> AbilitySystemClass;
	/*
	* The class used for the firearm's AttributeSet. This mus be a subclass of ModularFirearmAttributeSet.
	*/UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UModularFirearmAttributeSet> AttributeSetClass;
	/*
	* The currently equipped skins. Saved so that when component swapping occurs the correct skin is applied.
	*/UPROPERTY(Replicated, meta = (ArraySizeEnum = "EFirearmComponentType"))
	TArray<FString> ComponentSkins;
	/*
	* The firearm's AttributeSet. This mus be a subclass of ModularFirearmAttributeSet.
	* This component is created during OnConstruction so is not visible in the editor.
	*/UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UModularFirearmAttributeSet> AttributeSet;
	/*
	* The firearm's Ability System Component.
	* This is created during OnConstruction so is not visible in the editor.
	*/UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
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
	float DefaultSpreadMultiplier = 1.f;
	/*
	* 3D sound played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	USoundBase* DefaultFiringSound;
	/*
	* Niagara effect played when firing.
	*/UPROPERTY(EditAnywhere, Category = "Firearm|ComponentFallbacks|Barrel+Muzzle")
	UNiagaraSystem* DefaultMuzzleFlash;
#pragma endregion
#pragma region Cosmetics
	/*
	* Skin used by default.
	* Skins are applied to the 0 material slot.
	*/UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm|Cosmetics")
	FString DefaultSkin = "Normal";
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
	void PlayReplicatedMontage(UAnimMontage* montage, const FString& info = "");
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
	* Set to true when Reload is called. Set to false when the reload is stopped using the OnReceiverMontageEnded function.
	*/UPROPERTY()
	bool bReloading = false;
	/*
	* Sends the reload request to the server for replication.
	*/UFUNCTION(Server, Reliable)
	void ReloadOnServer(bool start = true);
	/*
	* Spawns the bullet on the server for replication.
	*/UFUNCTION(Server, Reliable)
	void SpawnBullet(const FVector& targetDirection);
	/*
	* Reduces the CurrentAmmo
	*/UFUNCTION(Server, Reliable)
	void ReduceCurrentAmmo();
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
	UPROPERTY(EditDefaultsOnly, Category = "Firearm|Parts", meta = (EditCondition = "!bUseSimpleGun", EditConditionHides, DisplayThumbnail = "false"), Replicated, ReplicatedUsing = OnRep_Muzzle)
	TObjectPtr<UGunMuzzleData> Muzzle;
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
	// Quick way to grab part data assets when looping.
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
	// Quick way to set part data assets when looping.
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
