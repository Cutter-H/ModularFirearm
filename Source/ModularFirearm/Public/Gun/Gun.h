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
		FString name = "Firearm";
	UPROPERTY(EditAnywhere, Category = "Info")
		FString description = "A gun.";
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

UCLASS()
class MODULARFIREARM_API AModularFirearm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModularFirearm();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	// Visible Components
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* ReceiverMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* AttachmentMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BarrelMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* GripMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* MagazineMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* SightMesh;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StockMesh;

	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
		FString name = "Firearm";
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
		FString description = "A gun.";
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
		UMaterialInstance* Icon;
	UPROPERTY(EditAnywhere, Category = "Info", meta = (DisplayPriority = 1))
		class UModularFirearmData* DefaultParts;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info", meta = (DisplayPriority = 1))
		FName AttachmentBoneName = "Attachment";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info", meta = (DisplayPriority = 1))
		FName BarrelBoneName = "Barrel";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info", meta = (DisplayPriority = 1))
		FName GripBoneName = "Grip";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info", meta = (DisplayPriority = 1))
		FName MagazineBoneName = "Magazine";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info", meta = (DisplayPriority = 1))
		FName SightBoneName = "Sight";
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Info", meta = (DisplayPriority = 1))
		FName StockBoneName = "Stock"; 
	
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
		float GunDamage = 1.f;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
		float RoundsPerSecond = 5.f;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
		bool bAutomatic = true;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (DisplayPriority = 2))
		TSubclassOf<AActor> BulletClass;

	UPROPERTY(ReplicatedUsing = OnRep_Attachment, BlueprintReadWrite, Category = "Modular Parts")
		class UGunAttachment* Attachment;
	UFUNCTION()
		void OnRep_Attachment();

	UPROPERTY(ReplicatedUsing = OnRep_Barrel, BlueprintReadWrite, Category = "Modular Parts")
		class UGunBarrel* Barrel;
	UFUNCTION()
		void OnRep_Barrel();

	UPROPERTY(ReplicatedUsing = OnRep_Grip, BlueprintReadWrite, Category = "Modular Parts")
		class UGunGrip* Grip;
	UFUNCTION()
		void OnRep_Grip();

	UPROPERTY(ReplicatedUsing = OnRep_Magazine, BlueprintReadWrite, Category = "Modular Parts")
		class UGunMagazine* Magazine;
	UFUNCTION()
		void OnRep_Magazine();

	UPROPERTY(ReplicatedUsing = OnRep_Sight, BlueprintReadWrite, Category = "Modular Parts")
		class UGunSight* Sight;
	UFUNCTION()
		void OnRep_Sight();

	UPROPERTY(ReplicatedUsing = OnRep_Stock, BlueprintReadWrite, Category = "Modular Parts")
		class UGunStock* Stock;
	UFUNCTION()
		void OnRep_Stock();

	bool LoadFirearmFromSaveSlot(FString saveSlotName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
