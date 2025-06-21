// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularFirearmTypes.generated.h"

USTRUCT()
struct FFirearmMeshComponents{
	GENERATED_BODY()
	FFirearmMeshComponents() {}
	UPROPERTY()
	USkeletalMeshComponent* Receiver = nullptr;
	UPROPERTY()
	UStaticMeshComponent* Barrel = nullptr;
	UPROPERTY()
	UStaticMeshComponent* Muzzle = nullptr;
	UPROPERTY()
	UStaticMeshComponent* Stock = nullptr;
	UPROPERTY()
	UStaticMeshComponent* Grip = nullptr;
	UPROPERTY()
	UStaticMeshComponent* Sight = nullptr;
	UPROPERTY()
	UStaticMeshComponent* Magazine = nullptr;
};

USTRUCT(BlueprintType)
struct FFirearmMaterialArray {
	GENERATED_BODY()
	FFirearmMaterialArray() {}
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSoftObjectPtr<UMaterialInterface>> Materials;
};

UENUM(BlueprintType)
enum EFirearmComponentType : uint8{
	MFPT_Receiver	=	0		UMETA(DisplayName="Receiver"),
	MFPT_Barrel		=	1		UMETA(DisplayName="Barrel"),
	MFPT_Grip		=	2		UMETA(DisplayName="Grip"),
	MFPT_Magazine	=	3		UMETA(DisplayName="Magazine"),
	MFPT_Sight		=	4		UMETA(DisplayName="Sight"),
	MFPT_Stock		=	5		UMETA(DisplayName="Stock"),
	MFPT_Muzzle		=	6		UMETA(DisplayName="Muzzle"),
	MFPT_Num		=	7		UMETA(Hidden)
};

UENUM(BlueprintType)
enum EFiringMode : uint8 {
	MFFM_Automatic = 0				UMETA(DisplayName = "Automatic"),
	MFFM_SemiAutomatic = 1			UMETA(DisplayName = "Semi Automatic"),
	MFFM_Burst = 2					UMETA(DisplayName = "Burst"),
	MFFM_Num = 3					UMETA(Hidden)
};

UENUM(BlueprintType)
enum ETargetingMode : uint8 {
	MFTM_FocalPoint = 0				UMETA(DisplayName = "Focal Point"),
	MFTM_DirectionOfMuzzle = 1		UMETA(DisplayName = "Direction of Muzzle"),
	MFTM_CursorLocation = 2			UMETA(DisplayName = "Cursor Location"),
	MFTM_NUM = 3					UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FModularFirearmAmmo {
	GENERATED_BODY()

	FModularFirearmAmmo(){}
	
	UPROPERTY(BlueprintReadWrite)
	int CurrentAmmo = -1;
	UPROPERTY(BlueprintReadWrite)
	int MaxAmmo = -1;
	UPROPERTY(BlueprintReadWrite)
	int ReserveAmmo = -1;
	
};

