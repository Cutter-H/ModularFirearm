// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoPouch.generated.h"

UENUM(BlueprintType)
enum EAmmoType {
	Rifle,
	Pistol,
	Shotgun,
	Sniper,
	None // Leave this here as blank for final index
};

USTRUCT(BlueprintType)
struct FAmmoData {
	GENERATED_USTRUCT_BODY()
public:
	EAmmoType type;
	int amount;
	FAmmoData()
	{
		type = EAmmoType::None;
		amount = 0;
	}
	FAmmoData(EAmmoType newType)
	{
		type = newType;
		amount = 0;
	}
	FAmmoData(EAmmoType newType, int newAmount)
	{
		type = newType;
		amount = newAmount;
	}

	inline bool operator== (const EAmmoType& other) const
	{
		return (type == other);
	}

	inline bool operator== (const FAmmoData& other) const
	{
		return (type == other.type);
	}

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoTypeChange, EAmmoType, Type, int, newAmount);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MODULARFIREARM_API UAmmoPouch : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmoPouch();
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnAmmoTypeChange OnAmmoTypeChange;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		bool bDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		TArray<FAmmoData> StartingAmmo;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		void ChangeAmmoType(TEnumAsByte<EAmmoType> changedType, int difference);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<FAmmoData> AmmoInventory;
	void GenerateEmptyAmmoPouch();

	void GenerateFilledAmmoPouch();

	void SetStartingAmmo();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
