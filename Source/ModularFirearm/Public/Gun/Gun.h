// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularFirearmDataAssets.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangeSignature, int, newAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletSpawnSignature, AActor*, newBullet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimationSignature, UAnimMontage*, montage);

UCLASS()
class MODULARFIREARM_API AModularFirearm : public AActor
{
	GENERATED_BODY()
	
public:	
	FOnAmmoChangeSignature OnCurrentAmmoChange;
	FOnBulletSpawnSignature OnBulletSpawn;
	FOnPlayAnimationSignature OnFiringMontagePlay;
	FOnPlayAnimationSignature OnReloadMontagePlay;
	FOnPlayAnimationSignature OnReloadMontageStop;
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

	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetCurrentAmmo() const { return CurrentMagazineAmmo + bBulletLoaded; }
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	int GetMaxAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Firearm|Getters")
	float GetFireRate() const;
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

protected:
#pragma region Firearm Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1))
	FString FirearmName = "Firearm";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1))
	FString FirearmDescription = "A gun.";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1))
	TObjectPtr<UMaterialInstance> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1))
	FString DefaultSkin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1))
	TMap<FString, UMaterialInterface*> Skins;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1, ExposeOnSpawn = "true"))
	TObjectPtr<UModularFirearmData> DefaultParts;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1, ExposeOnSpawn = "true"))
	bool bStartWithWeaponLoaded = true;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	FScalableFirearmFloat GunDamageMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1))
	FScalableFirearmFloat MultiShot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (DisplayPriority = 1), AdvancedDisplay)
	bool bPlayMontagesFromExternalSource = false;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	TEnumAsByte<EFiringType> FiringType;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	TEnumAsByte<ETargetingMode> TargetingMode;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	TEnumAsByte<ECollisionChannel> TargetingChannel = ECollisionChannel::ECC_Visibility;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	bool bContinuousFire = true;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	FScalableFirearmFloat RoundsPerSecond = 5.f;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2), AdvancedDisplay)
	bool bRecycleAmmoOnReload = true;
#pragma endregion
#pragma region Cosmetics
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetics")
	TObjectPtr<UAnimMontage> FiringMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetics")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UFUNCTION(NetMulticast, Reliable)
	void PlayReplicatedMontage(UAnimMontage* montage, const FString& info = "");
	UFUNCTION()
	void OnReceiverMontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion
#pragma region Attachment Names
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info|AttachmentNames", meta = (DisplayPriority = 1))
	FName AttachmentBoneName = "Attachment";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info|AttachmentNames", meta = (DisplayPriority = 1))
	FName BarrelBoneName = "Barrel";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info|AttachmentNames", meta = (DisplayPriority = 1))
	FName GripBoneName = "Grip";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info|AttachmentNames", meta = (DisplayPriority = 1))
	FName MagazineBoneName = "Magazine";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info|AttachmentNames", meta = (DisplayPriority = 1))
	FName SightBoneName = "Sight";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info|AttachmentNames", meta = (DisplayPriority = 1))
	FName StockBoneName = "Stock";
#pragma endregion
	AModularFirearm();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
#pragma region Bullet Functionality
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentAmmo)
	bool bBulletLoaded = false;
	UPROPERTY(Replicated)
	int CurrentMagazineAmmo = 0;
	UPROPERTY()
	FTimerHandle FiringTimer;
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
	void FireWeapon();
	UFUNCTION()
	void LoadNewMagazine(bool bFreeFill = false);
#pragma endregion
	UPROPERTY(Replicated, meta = (ArraySizeEnum = "EFirearmComponentType"))
	TArray<FString> ComponentSkins;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_FirearmLevel)
	int FirearmLevel = 1;
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateSkinChange(const EFirearmComponentType& componentType, const FString& skinName);
	UFUNCTION()
	void OnRep_FirearmLevel();
	UFUNCTION(Server, Reliable)
	void SetComponentOnServer(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent);
	UFUNCTION(Server, Reliable)
	void SetComponentSkinOnServer(const EFirearmComponentType& componentType, const FString& skinName);
	UFUNCTION()
	void UpdateSkin(const EFirearmComponentType& componentType, const FString& skinName);


#pragma region Mesh Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> ReceiverMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> AttachmentMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> BarrelMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> GripMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MagazineMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SightMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<USkeletalMeshComponent> StockMesh;
#pragma endregion
#pragma region Component Data
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Attachment)
	TObjectPtr<UGunAttachmentData> Attachment;
	UFUNCTION()
	void OnRep_Attachment();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Barrel)
	TObjectPtr<UGunBarrelData> Barrel;
	UFUNCTION()
	void OnRep_Barrel();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Grip)
	TObjectPtr<UGunGripData> Grip;
	UFUNCTION()
	void OnRep_Grip();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Magazine)
	TObjectPtr<UGunMagazineData> Magazine;
	UFUNCTION()
	void OnRep_Magazine();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Sight)
	TObjectPtr<UGunSightData> Sight;
	UFUNCTION()
	void OnRep_Sight();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Stock)
	TObjectPtr<UGunStockData> Stock;
	UFUNCTION()
	void OnRep_Stock();
#pragma endregion
};
