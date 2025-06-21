// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun/ModularFirearmCustomizationComponent.h"
#include "Gun/ModularFirearmDataAssets.h"
#include "ModularFirearmPlugin.h"
#include "Engine/AssetManager.h"
#include "Gun/ModularFirearm.h"
#include "Net/UnrealNetwork.h"


UModularFirearmCustomizationComponent::UModularFirearmCustomizationComponent() {
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UModularFirearmCustomizationComponent::SetMeshComponents(FFirearmMeshComponents components) {
	if (components.Receiver && !ReceiverMesh) {
		ReceiverMesh = components.Receiver;
	}
#define ASSIGNPART(x)\
	if (components.x && !FirearmComponents.Contains(EFirearmComponentType::MFPT_##x)) {\
		FirearmComponents.Add(EFirearmComponentType::MFPT_##x, components.x);\
	}
	ASSIGNPART(Grip);
	ASSIGNPART(Sight);
	ASSIGNPART(Magazine);
	ASSIGNPART(Stock);
	ASSIGNPART(Barrel);
	ASSIGNPART(Muzzle);
#undef ASSIGNPART
}

void UModularFirearmCustomizationComponent::ChangePart(EFirearmComponentType type, UGunPartDataBase* data, bool async) {
	if (type == EFirearmComponentType::MFPT_Receiver) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Unable to change the receiver component. Please change the firearm instead."));
		return;
	}
	if (type == EFirearmComponentType::MFPT_Num) {
		return;
	}
	if (data) {
		const auto& newMesh = data->Mesh;
		const auto& meshComp = FirearmComponents[type];

		if (!async) {
			meshComp->SetStaticMesh(newMesh.LoadSynchronous());
			return;
		}
		
		FStreamableManager& streamableManager = UAssetManager::GetStreamableManager();
		streamableManager.RequestAsyncLoad(newMesh.ToSoftObjectPath(), [newMesh, meshComp]() {
			meshComp->SetStaticMesh(newMesh.LoadSynchronous());
		}
		);
		
		return;
	}
	FirearmComponents[type]->SetStaticMesh(nullptr);
}

void UModularFirearmCustomizationComponent::ChangeSkin(EFirearmComponentType type, FString skin, bool async) {
	if (type == EFirearmComponentType::MFPT_Num) { return; }
	UMeshComponent* meshComp;
	if (type == EFirearmComponentType::MFPT_Receiver) { meshComp = ReceiverMesh; }
	else { meshComp = FirearmComponents[type]; }
	
	
	if (Firearm->GetPartData(type)->Skins.Contains(skin)) {
		const auto& mats = Firearm->GetPartData(type)->Skins[skin].Materials;
		for ( int i=0; i < mats.Num() && i < meshComp->GetNumMaterials(); i++ ) {
			const TSoftObjectPtr<UMaterialInterface>& newSkin = mats[i];
			if (!async) {
				meshComp->SetMaterial(i, newSkin.LoadSynchronous());
				return;
			}

			FStreamableManager& streamableManager = UAssetManager::GetStreamableManager();
			streamableManager.RequestAsyncLoad(newSkin.ToSoftObjectPath(), [i, newSkin, meshComp]() {
				if (newSkin) { // Check here because validation may fail for unloaded assets.
					meshComp->SetMaterial(i, newSkin.LoadSynchronous());	
				}
			}
			);
		}
	}
}

void UModularFirearmCustomizationComponent::BeginPlay() {
	Super::BeginPlay();
	if (GetOwner()->HasAuthority()) {
		Firearm = Cast<AModularFirearm>(GetOwner());
		if (!Firearm) { DestroyComponent(); }
		DefaultSkin = Firearm->DefaultSkin;
	}
}

void UModularFirearmCustomizationComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UModularFirearmCustomizationComponent, Firearm);
	DOREPLIFETIME(UModularFirearmCustomizationComponent, DefaultSkin);
}

