// Cutter Hodnett // 2022-


#include "Gun/GunComponent.h"

// Sets default values for this component's properties
UModularFirearmComponent::UModularFirearmComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UModularFirearmComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UModularFirearmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UStaticMesh* UModularFirearmComponent::GetMesh() { return Mesh; }