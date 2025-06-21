// Cutter Hodnett // 2022-


#include "Ammo/AmmoPouch.h"

// Sets default values for this component's properties
UAmmoPouch::UAmmoPouch()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAmmoPouch::ChangeAmmoType(TEnumAsByte<EAmmoType> changedType, int difference)
{
	for (FAmmoData data : AmmoInventory)
	{
		if (data == changedType)
		{
			data.amount += difference;
			OnAmmoTypeChange.Broadcast(data.type, data.amount);
			return;
		}
	}
}

// Called when the game starts
void UAmmoPouch::BeginPlay()
{
	Super::BeginPlay();

	if (bDebug)
		GenerateFilledAmmoPouch();
	else
		GenerateEmptyAmmoPouch();

	SetStartingAmmo();
	
}

void UAmmoPouch::GenerateEmptyAmmoPouch()
{
	for (int i = EAmmoType::Rifle; i != None; i++)
	{
		AmmoInventory.Add(FAmmoData(static_cast<EAmmoType>(i)));
	}
}

void UAmmoPouch::GenerateFilledAmmoPouch()
{
	for (int i = EAmmoType::Rifle; i != None; i++)
	{
		AmmoInventory.Add(FAmmoData(static_cast<EAmmoType>(i), 1000));
	}
}

void UAmmoPouch::SetStartingAmmo()
{
	for (FAmmoData startingAmmoData : StartingAmmo)
	{
		for (FAmmoData ammoData : AmmoInventory)
		{
			if (ammoData == startingAmmoData)
			{
				ammoData.amount = startingAmmoData.amount;
			}
		}
	}
}

// Called every frame
void UAmmoPouch::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}