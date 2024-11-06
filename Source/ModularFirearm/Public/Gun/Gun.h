// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

USTRUCT(BlueprintType)
struct FGunData
{
public:
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Info")
		FString FirearmName = "Firearm";
	UPROPERTY(EditAnywhere, Category = "Info")
		FString FirearmDescription = "A gun.";
	UPROPERTY(EditAnywhere, Category = "Info")
		UMaterialInstance* Icon;
	UPROPERTY(EditAnywhere, Category = "Info")
		class UModularFirearmData* DefaultComponents;

	

	UPROPERTY(EditAnywhere, Category = "Stats")
		float GunDamage = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float RoundsPerSecond = 5.f;
	UPROPERTY(EditAnywhere, Category = "Stats")
		bool bAutomatic = true;
	UPROPERTY(EditAnywhere, Category = "Stats")
		TSubclassOf<AActor> BulletClass;
	
	

};

UENUM(BlueprintType)
enum EFirearmComponentType {
	Attachment,
	Barrel,
	Grip,
	Magazine,
	Sight,
	Stock
};

class UModularFirearmData;
class UGunPartDataBase;
class UGunAttachmentData;
class UGunBarrelData;
class UGunGripData;
class UGunMagazineData;
class UGunSightData;
class UGunStockData;


UCLASS()
class MODULARFIREARM_API AModularFirearm : public AActor
{
	GENERATED_BODY()
	
public:	
	AModularFirearm();

	UFUNCTION(BlueprintCallable, Category = "Firearm|Attachment")
	void SetComponent(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent);

	UFUNCTION(BlueprintCallable, Category = "Firearm|Attachment")
	void SetComponentSkin(const EFirearmComponentType& componentType, const FString& skinName);

	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	void BeginFiring();
	UFUNCTION(BlueprintCallable, Category = "Firearm|Firing")
	void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Firearm|Reload")
	void Reload();

protected:
#pragma region Firearm Variables
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
	FString FirearmName = "Firearm";
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
	FString FirearmDescription = "A gun.";
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
	TObjectPtr<UMaterialInstance> Icon;
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
	FString DefaultSkin;
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
	TObjectPtr<UModularFirearmData> DefaultParts;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	float GunDamage = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	float RoundsPerSecond = 5.f;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	bool bAutomatic = true;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
	TSubclassOf<AActor> BulletClass;
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
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentSkins)
	TArray<FString> CurrentSkins;
	UFUNCTION()
	void OnRep_CurrentSkins(TArray<FString> oldArray);

	UFUNCTION(Server, Reliable)
	void SetComponentOnServer(EFirearmComponentType componentType, UGunPartDataBase* newComponent);
	UFUNCTION(Server, Reliable)
	void SetComponentSkinOnServer(EFirearmComponentType componentType, const FString& skinName);

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
