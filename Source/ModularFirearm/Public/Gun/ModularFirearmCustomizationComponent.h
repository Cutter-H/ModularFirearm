// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun/ModularFirearmTypes.h"
#include "Components/ActorComponent.h"
#include "ModularFirearmCustomizationComponent.generated.h"

class AModularFirearm;



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MODULARFIREARM_API UModularFirearmCustomizationComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UModularFirearmCustomizationComponent();
	UFUNCTION()
	void SetMeshComponents(FFirearmMeshComponents components);
	UFUNCTION()
	void ChangePart(EFirearmComponentType type, UGunPartDataBase* data, bool async = true);
	UFUNCTION()
	void ChangeSkin(EFirearmComponentType type, FString skin, bool async = true);
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> ReceiverMesh;
	UPROPERTY()
	TMap<TEnumAsByte<EFirearmComponentType>, TObjectPtr<UStaticMeshComponent>> FirearmComponents;
	UPROPERTY(Replicated)
	FString DefaultSkin;
	UPROPERTY(Replicated)
	TObjectPtr<AModularFirearm> Firearm;
	
};
