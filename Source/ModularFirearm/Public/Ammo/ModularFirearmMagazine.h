// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularFirearmMagazine.generated.h"

class UGunMagazineData;
class AModularFirearm;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagazineAmmoChangeSignature, int, newAmmo);
UCLASS()
class MODULARFIREARM_API AModularFirearmMagazine : public AActor {
	GENERATED_BODY()

public:
	AModularFirearmMagazine();
	UPROPERTY(BlueprintAssignable)
	FOnMagazineAmmoChangeSignature OnMagazineAmmoChange;
	
	UFUNCTION(BlueprintCallable, Category = "Magazine")
	UStaticMeshComponent* GetMesh() const { return Mesh; }

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Magazine")
	void Morph(UGunMagazineData* newData, bool bFill = false);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(Replicated)
	int Ammo;

	UPROPERTY(Replicated)
	TObjectPtr<UGunMagazineData> Data;

	UFUNCTION(NetMulticast, Reliable)
	void ReplicateMorph(UGunMagazineData* newData);
	
};
