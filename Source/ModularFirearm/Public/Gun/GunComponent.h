// Cutter Hodnett // 2022-

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDismemberment, FName, BoneDismembered);

UCLASS( ClassGroup=(Custom) )
class MODULARFIREARM_API UModularFirearmComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UModularFirearmComponent();
	
	UFUNCTION()
	UStaticMesh* GetMesh();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
		FString name = "Gun Component";
	UPROPERTY(EditAnywhere)
		FString description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
	UPROPERTY(EditAnywhere)
		UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInterface*> AlternativeSkins;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* Icon;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
