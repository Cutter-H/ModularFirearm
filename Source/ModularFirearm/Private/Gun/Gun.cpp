// Cutter Hodnett // 2022-


#include "Gun/Gun.h"
#include "Net/UnrealNetwork.h"

#include "ModularFirearmDataAssets.h"
#include "Gun/GunComponent.h"
#include "Gun/GunAttachment.h"
#include "Gun/GunBarrel.h"
#include "Gun/GunGrip.h"
#include "Gun/GunMagazine.h"
#include "Gun/GunSight.h"
#include "Gun/GunStock.h"

// Sets default values
AModularFirearm::AModularFirearm() {
	PrimaryActorTick.bCanEverTick = false;

	ReceiverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverSkeletalMesh"));
	SetRootComponent(ReceiverMesh);

	AttachmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AttachmentMesh"));
	AttachmentMesh->SetupAttachment(ReceiverMesh, AttachmentBoneName);

	BarrelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetupAttachment(ReceiverMesh, BarrelBoneName);

	GripMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GripMesh"));
	GripMesh->SetupAttachment(ReceiverMesh, GripBoneName);

	MagazineMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(ReceiverMesh, MagazineBoneName);

	SightMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SightMesh"));
	SightMesh->SetupAttachment(ReceiverMesh, SightBoneName);

	StockMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StockMesh"));
	StockMesh->SetupAttachment(ReceiverMesh, StockBoneName);
	
}

void AModularFirearm::SetComponent(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent) {
	if (!HasAuthority() && !GetIsReplicated()) {
		SetComponentOnServer(componentType, newComponent);
		return;
	}
	switch (componentType) {
	case EFirearmComponentType::Attachment: {
		Attachment = Cast<UGunAttachmentData>(newComponent);
		if (Attachment) {
			if (Attachment->Mesh) {
				AttachmentMesh->SetSkeletalMesh(Attachment->Mesh);
				if (IsValid(Attachment->DefaultAnimInstance)) {
					AttachmentMesh->SetAnimInstanceClass(Attachment->DefaultAnimInstance);
				}
				return;
			}
		}
		AttachmentMesh->SetSkeletalMesh(nullptr);
		return;
	}
	case EFirearmComponentType::Barrel: {
		Barrel = Cast<UGunBarrelData>(newComponent);
		if (Barrel) {
			if (Barrel->Mesh) {
				BarrelMesh->SetSkeletalMesh(Barrel->Mesh);
				if (IsValid(Barrel->DefaultAnimInstance)) {
					BarrelMesh->SetAnimInstanceClass(Barrel->DefaultAnimInstance);
				}
				return;
			}
		}
		BarrelMesh->SetSkeletalMesh(nullptr);
		return;
	}
	case EFirearmComponentType::Grip: {
		Grip = Cast<UGunGripData>(newComponent);
		if (Grip) {
			if (Grip->Mesh) {
				GripMesh->SetSkeletalMesh(Grip->Mesh);
				if (IsValid(Grip->DefaultAnimInstance)) {
					GripMesh->SetAnimInstanceClass(Grip->DefaultAnimInstance);
				}
				return;
			}
		}
		GripMesh->SetSkeletalMesh(nullptr);
		return;
	}
	case EFirearmComponentType::Magazine: {
		Magazine = Cast<UGunMagazineData>(newComponent);
		if (Magazine) {
			if (Magazine->Mesh) {
				MagazineMesh->SetSkeletalMesh(Magazine->Mesh);
				if (IsValid(Magazine->DefaultAnimInstance)) {
					MagazineMesh->SetAnimInstanceClass(Magazine->DefaultAnimInstance);
				}
				return;
			}
		}
		MagazineMesh->SetSkeletalMesh(nullptr);
		return;
	}
	case EFirearmComponentType::Sight: {
		Sight = Cast<UGunSightData>(newComponent);
		if (Sight) {
			if (Sight->Mesh) {
				SightMesh->SetSkeletalMesh(Sight->Mesh);
				if (IsValid(Sight->DefaultAnimInstance)) {
					SightMesh->SetAnimInstanceClass(Sight->DefaultAnimInstance);
				}
				return;
			}
		}
		SightMesh->SetSkeletalMesh(nullptr);
		return;
	}
	case EFirearmComponentType::Stock: {
		Stock = Cast<UGunStockData>(newComponent);
		if (Stock) {
			if (Stock->Mesh) {
				StockMesh->SetSkeletalMesh(Stock->Mesh);
				if (IsValid(Stock->DefaultAnimInstance)) {
					StockMesh->SetAnimInstanceClass(Stock->DefaultAnimInstance);
				}
				return;
			}
		}
		StockMesh->SetSkeletalMesh(nullptr);
		return;
	}
	}
}
void AModularFirearm::SetComponentSkin(const EFirearmComponentType& componentType, const FString& skinName) {
	if (!HasAuthority() && !GetIsReplicated()) {
		SetComponentSkinOnServer(componentType, skinName);
		return;
	}
	switch (componentType) {
	case EFirearmComponentType::Attachment: {
		if (IsValid(AttachmentMesh) && IsValid(Attachment)) {
			if (UMaterialInterface* newMaterial = Attachment->Skins.FindRef(skinName)) {
				AttachmentMesh->SetMaterial(0, newMaterial);
				CurrentSkins[0] = skinName;
			}
		}
		return;
	}
	case EFirearmComponentType::Barrel: {
		if (IsValid(BarrelMesh) && IsValid(Barrel)) {
			if (UMaterialInterface* newMaterial = Barrel->Skins.FindRef(skinName)) {
				BarrelMesh->SetMaterial(0, newMaterial);
				CurrentSkins[1] = skinName;
			}
		}
		return;
	}
	case EFirearmComponentType::Grip: {
		if (IsValid(GripMesh) && IsValid(Grip)) {
			if (UMaterialInterface* newMaterial = Grip->Skins.FindRef(skinName)) {
				GripMesh->SetMaterial(0, newMaterial);
				CurrentSkins[2] = skinName;
			}
		}
		return;
	}
	case EFirearmComponentType::Magazine: {
		if (IsValid(MagazineMesh) && IsValid(Magazine)) {
			if (UMaterialInterface* newMaterial = Magazine->Skins.FindRef(skinName)) {
				MagazineMesh->SetMaterial(0, newMaterial);
				CurrentSkins[3] = skinName;
			}
		}
		return;
	}
	case EFirearmComponentType::Sight: {
		if (IsValid(SightMesh) && IsValid(Sight)) {
			if (UMaterialInterface* newMaterial = Sight->Skins.FindRef(skinName)) {
				SightMesh->SetMaterial(0, newMaterial);
				CurrentSkins[4] = skinName;
			}
		}
		return;
	}
	case EFirearmComponentType::Stock: {
		if (IsValid(StockMesh) && IsValid(Stock)) {
			if (UMaterialInterface* newMaterial = Stock->Skins.FindRef(skinName)) {
				StockMesh->SetMaterial(0, newMaterial);
				CurrentSkins[5] = skinName;
			}
		}
		return;
	}
	}
}

void AModularFirearm::OnConstruction(const FTransform& Transform) {
	if (DefaultParts) {
		if (DefaultParts->Attachment)
			if (DefaultParts->Stock->Mesh)
				AttachmentMesh->SetSkeletalMesh(DefaultParts->Attachment->Mesh);
		if (DefaultParts->Barrel)
			if (DefaultParts->Barrel->Mesh)
				BarrelMesh->SetSkeletalMesh(DefaultParts->Barrel->Mesh);
		if (DefaultParts->Grip)
			if (DefaultParts->Grip->Mesh)
				GripMesh->SetSkeletalMesh(DefaultParts->Grip->Mesh);
		if (DefaultParts->Magazine)
			if (DefaultParts->Magazine->Mesh)
				MagazineMesh->SetSkeletalMesh(DefaultParts->Magazine->Mesh);
		if (DefaultParts->Sight)
			if (DefaultParts->Sight->Mesh)
				SightMesh->SetSkeletalMesh(DefaultParts->Sight->Mesh);
		if (DefaultParts->Stock)
			if (DefaultParts->Stock->Mesh)
				StockMesh->SetSkeletalMesh(DefaultParts->Stock->Mesh);
	}
	FString skinName = DefaultSkin;
	if (IsValid(DefaultParts)) {
		skinName = DefaultParts->DefaultSkin;
	}
	if (Attachment) {
		UMaterialInterface* newMaterial = Attachment->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(AttachmentMesh)) {
			AttachmentMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Barrel) {
		UMaterialInterface* newMaterial = Barrel->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(BarrelMesh)) {
			BarrelMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Grip) {
		UMaterialInterface* newMaterial = Grip->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(GripMesh)) {
			GripMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Magazine) {
		UMaterialInterface* newMaterial = Magazine->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(MagazineMesh)) {
			MagazineMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Sight) {
		UMaterialInterface* newMaterial = Sight->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(SightMesh)) {
			SightMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Stock) {
		UMaterialInterface* newMaterial = Stock->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(StockMesh)) {
			StockMesh->SetMaterial(0, newMaterial);
		}
	}
	for (int i = 0; i <= 5; i++) {
		CurrentSkins.Add(skinName);
	}

}
void AModularFirearm::BeginPlay() {
	Super::BeginPlay();
	
}
void AModularFirearm::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, CurrentSkins, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Attachment, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Barrel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Grip, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Magazine, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Sight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Stock, COND_None, REPNOTIFY_Always);
}

void AModularFirearm::OnRep_CurrentSkins(TArray<FString> oldArray) {
	for (int i = 0; i < FMath::Min(oldArray.Num(), CurrentSkins.Num()); i++) {
		FString skinName = CurrentSkins[i];
		if (oldArray[i] != skinName) {
			switch (i) {
			case 0: {
				if (IsValid(Attachment)) {
					if (UMaterialInterface* newMaterial = Attachment->Skins.FindRef(skinName)) {
						if (AttachmentMesh) {
							AttachmentMesh->SetMaterial(0, newMaterial);
						}
					}
				}
				return;
			}
			case 1: {
				if (IsValid(Barrel)) {
					if (UMaterialInterface* newMaterial = Barrel->Skins.FindRef(skinName)) {
						if (BarrelMesh) {
							BarrelMesh->SetMaterial(0, newMaterial);
						}
					}
				}
				return;
			}
			case 2: {
				if (IsValid(Grip)) {
					if (UMaterialInterface* newMaterial = Grip->Skins.FindRef(skinName)) {
						if (GripMesh) {
							GripMesh->SetMaterial(0, newMaterial);
						}
					}
				}
				return;
			}
			case 3: {
				if (IsValid(Magazine)) {
					if (UMaterialInterface* newMaterial = Magazine->Skins.FindRef(skinName)) {
						if (AttachmentMesh) {
							AttachmentMesh->SetMaterial(0, newMaterial);
						}
					}
				}
				return;
			}
			case 4: {
				if (IsValid(Sight)) {
					if (UMaterialInterface* newMaterial = Sight->Skins.FindRef(skinName)) {
						if (SightMesh) {
							SightMesh->SetMaterial(0, newMaterial);
						}
					}
				}
				return;
			}
			case 5: {
				if (IsValid(Stock)) {
					if (UMaterialInterface* newMaterial = Stock->Skins.FindRef(skinName)) {
						if (StockMesh) {
							StockMesh->SetMaterial(0, newMaterial);
						}
					}
				}
				return;
			}
			default:
				return;
			}
		}
	}
}
void AModularFirearm::SetComponentOnServer_Implmentation(EFirearmComponentType componentType, UGunPartDataBase* newComponent) {
	if (HasAuthority()) {
		SetComponent(componentType, newComponent);
	}
}
void AModularFirearm::SetComponentSkinOnServer_Implmentation(EFirearmComponentType componentType, const FString& skinName) {
	if (HasAuthority()) {
		SetComponentSkin(componentType, skinName);
	}
}
// Component On Rep functions
void AModularFirearm::OnRep_Attachment()
{
	if (IsValid(Attachment) && IsValid(AttachmentMesh))
	{
		AttachmentMesh->SetSkeletalMesh(Attachment->Mesh);
		if (IsValid(Attachment->DefaultAnimInstance)) {
			AttachmentMesh->SetAnimInstanceClass(Attachment->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Barrel()
{
	if(IsValid(Barrel) && IsValid(BarrelMesh)) {
		BarrelMesh->SetSkeletalMesh(Barrel->Mesh);
		if (IsValid(Barrel->DefaultAnimInstance)) {
			BarrelMesh->SetAnimInstanceClass(Barrel->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Grip()
{
	if(IsValid(Grip) && IsValid(GripMesh)) {
		GripMesh->SetSkeletalMesh(Grip->Mesh);
		if (IsValid(Grip->DefaultAnimInstance)) {
			GripMesh->SetAnimInstanceClass(Grip->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Magazine()
{
	if(IsValid(Magazine) && IsValid(MagazineMesh)) {
		MagazineMesh->SetSkeletalMesh(Magazine->Mesh);
		if (IsValid(Magazine->DefaultAnimInstance)) {
			MagazineMesh->SetAnimInstanceClass(Magazine->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Sight()
{
	if (IsValid(Sight) && IsValid(SightMesh)) {
		SightMesh->SetSkeletalMesh(Sight->Mesh);
		if (IsValid(Sight->DefaultAnimInstance)) {
			SightMesh->SetAnimInstanceClass(Sight->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Stock()
{
	if (IsValid(Stock) && IsValid(StockMesh)) {
		StockMesh->SetSkeletalMesh(Stock->Mesh);
		if (IsValid(Stock->DefaultAnimInstance)) {
			StockMesh->SetAnimInstanceClass(Stock->DefaultAnimInstance);
		}
	}
}
