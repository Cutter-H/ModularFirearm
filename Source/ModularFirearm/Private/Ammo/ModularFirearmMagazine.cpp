// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo/ModularFirearmMagazine.h"

#include "Gun/ModularFirearmDataAssets.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Net/UnrealNetwork.h"


AModularFirearmMagazine::AModularFirearmMagazine() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Magazine Mesh"));
	SetRootComponent(Mesh);
}

void AModularFirearmMagazine::Morph(UGunMagazineData* newData, bool bFill) {
	Data = newData;
	if (bFill) {
		Ammo = newData->GetCapacity();
	}
	else {
		Ammo = FMath::Min(Ammo, newData->GetCapacity());
	}
}

void AModularFirearmMagazine::BeginPlay() {
	Super::BeginPlay();
	
}

void AModularFirearmMagazine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AModularFirearmMagazine, Ammo);
	DOREPLIFETIME(AModularFirearmMagazine, Data);
	
}

void AModularFirearmMagazine::ReplicateMorph_Implementation(UGunMagazineData* newData) {
	if (!newData) {
		Mesh->SetStaticMesh(nullptr);
		return;
	}
	const auto newMesh = newData->Mesh;
	const auto meshComp = Mesh;
	FStreamableManager& streamableManager = UAssetManager::GetStreamableManager();
	streamableManager.RequestAsyncLoad(newMesh.ToSoftObjectPath(), [meshComp, newMesh]() {
		if (meshComp && newMesh) {
			meshComp->SetStaticMesh(newMesh.LoadSynchronous());
		}
	});	
}
