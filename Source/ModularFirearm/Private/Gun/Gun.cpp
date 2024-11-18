// Cutter Hodnett // 2024


#include "Gun/Gun.h"
#include "ModularFirearm.h"
#include "AIController.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#pragma region Firing
void AModularFirearm::StartFiring() {
	bWantsToFire = true;
	if (GetWorld()->GetTimerManager().IsTimerActive(RecoilTimer)) {
		return;
	}
	if (FiringMode == EFiringMode::Automatic) {
		if (!GetWorld()->GetTimerManager().TimerExists(FiringTimer)) {
			float delay = 1 / GetFireRate();
			FTimerDelegate fireDel;
			fireDel.BindUObject(this, &AModularFirearm::FireWeapon, false);
			GetWorld()->GetTimerManager().SetTimer(FiringTimer, fireDel, delay, true);
		}
		else {
			GetWorld()->GetTimerManager().UnPauseTimer(FiringTimer);
		}
	}
	if (FiringMode == EFiringMode::Burst) {
		BurstFireWeapon(GetBurstAmount());
	}
	else {
		FireWeapon();
	}
}
void AModularFirearm::StopFiring() {
	bWantsToFire = false;
	if (GetWorld()->GetTimerManager().TimerExists(FiringTimer)) {
		GetWorld()->GetTimerManager().PauseTimer(FiringTimer);
	}
}
void AModularFirearm::SpawnBullet_Implementation(const FVector& targetLocation) {
	TSubclassOf<AActor> bulletClass = GetBulletClass();
	if (!IsValid(bulletClass)) {
		return;
	}
	FTransform spawnTransform = GetMuzzleTransform();
	FVector direction = UKismetMathLibrary::FindLookAtRotation(spawnTransform.GetLocation(), targetLocation).Vector();
	spawnTransform.SetRotation(FQuat::MakeFromRotator(direction.Rotation()));
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this; spawnParams.Instigator = GetInstigator(); spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* bullet = GetWorld()->SpawnActor<AActor>(bulletClass, spawnTransform, spawnParams);
	if (!IsValid(bullet)) {
		return;
	}
	OnBulletSpawn.Broadcast(bullet);
	PlayReplicatedMontage(FiringMontage, "Fire");
	if (CurrentMagazineAmmo > 0) {
		--CurrentMagazineAmmo;
	}
	else {
		bBulletChambered = false;
	}
}
void AModularFirearm::OnRep_CurrentAmmo() {
	OnCurrentAmmoChange.Broadcast(CurrentMagazineAmmo + bBulletChambered);
}
void AModularFirearm::FireWeapon(bool force) {
	if (!force && !bWantsToFire) {

		GetWorld()->GetTimerManager().PauseTimer(FiringTimer);
		VolleyBulletCount = 0;
		return;
	}
	if (GetCurrentAmmo() > 0) {
		FVector targetLocation;
		/* Setup Targeting Location */ {
			switch (TargetingMode) {
			case ETargetingMode::FocalPoint: {
				if (GetInstigator()) {
					if (GetInstigator()->IsPlayerControlled()) {
						APlayerController* playerCon = GetInstigator()->GetController<APlayerController>();
						if (APlayerCameraManager* camMan = playerCon->PlayerCameraManager) {
							FVector camLoc; FRotator camRot; camMan->GetCameraViewPoint(camLoc, camRot);
							camRot = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(camRot.Vector(), GetBulletSpread(VolleyBulletCount)).Rotation();
							FCollisionQueryParams hitParams; hitParams.AddIgnoredActor(GetOwner()); hitParams.AddIgnoredActor(this); hitParams.bTraceComplex = true;
							FHitResult hit;  if (GetWorld()->LineTraceSingleByChannel(hit, camLoc, camLoc + (camRot.Vector() * MAX_flt), TargetingChannel, hitParams)) {
								targetLocation = hit.ImpactPoint;
							}
							else {
								targetLocation = hit.TraceEnd;
							}
						}
					}
					else {
						targetLocation = GetInstigator()->GetController<AAIController>()->GetFocalPoint();
					}
				}
				break;
			}
			case ETargetingMode::DirectionOfMuzzle: {
				FRotator rot = UKismetMathLibrary::ComposeRotators(GetMuzzleTransform().GetRotation().Rotator(), MuzzleOffset);
				rot = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(rot.Vector(), GetBulletSpread(VolleyBulletCount)).Rotation();
				targetLocation = GetMuzzleTransform().GetLocation() + rot.Vector();
				break;
			}
			case ETargetingMode::CursorLocation: {
				APlayerController* playerCon = GetWorld()->GetFirstPlayerController();
				FHitResult hit;
				playerCon->GetHitResultUnderCursor(TargetingChannel, true, hit);
				if (hit.bBlockingHit) {
					targetLocation = hit.ImpactPoint;
				}
				else {
					targetLocation = hit.TraceEnd;
				}
				FVector dir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees((targetLocation - GetMuzzleTransform().GetLocation()).GetSafeNormal(), GetBulletSpread(VolleyBulletCount));
				targetLocation = GetMuzzleTransform().GetLocation() + dir;
				break;
			}
			}
		}
		VolleyBulletCount++;
		SpawnBullet(targetLocation);
		/* Effects */ {
			if (GetCamShake()) {
				if (IsValid(GetInstigator())) {
					if (APlayerController* pCon = GetInstigator()->GetController<APlayerController>()) {
						if (APlayerCameraManager* camMan = pCon->PlayerCameraManager) {
							camMan->StartCameraShake(GetCamShake(), GetCamShakeIntensity());
						}
					}
				}
			}
			if (GetHapticFeedback()) {
				UGameplayStatics::SpawnForceFeedbackAttached(GetHapticFeedback(), ReceiverMesh, FName(), FVector(), FRotator(), EAttachLocation::SnapToTarget, true, false, GetHapticIntensity());
			}
		}
		/* Setup Recoiling */ {
			FTimerDelegate recoilDel;
			float delay = 1 / GetFireRate();
			GetWorld()->GetTimerManager().SetTimer(RecoilTimer, recoilDel, delay, false);
		}

		
	}
}
void AModularFirearm::BurstFireWeapon(int burst) {
	if (burst <= 0) {
		return;
	}
	FireWeapon(true);
	int newBurst = burst - 1;
	if (newBurst > 0) {
		FTimerHandle burstHandle;
		FTimerDelegate burstDel;
		burstDel.BindUObject(this, &AModularFirearm::BurstFireWeapon, newBurst);
		float burstDelay = (1 / Eval(BurstSpeed));
		GetWorld()->GetTimerManager().SetTimer(burstHandle, burstDel, burstDelay, false);
	}
}
#pragma endregion
#pragma region Reloading
void AModularFirearm::StartReloading() {
	if (GetReserveAmmo() <= 0 || GetCurrentAmmo() <= GetMaxAmmo() || bReloading) {
		return;
	}
	ReloadOnServer(true);
}
void AModularFirearm::StopReloading() {
	ReloadOnServer(false);
}
void AModularFirearm::ReloadOnServer_Implementation(bool start) {
	if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
		bool reloadActive = animInst->Montage_IsPlaying(GetReloadMontage());
		if (!reloadActive) {
			return;
		}
	}
	PlayReplicatedMontage(GetReloadMontage(), "Reload");
}
void AModularFirearm::LoadNewMagazine(bool bFreeFill) {
	if (!HasAuthority() || GetReserveAmmo() <= 0) {
		return;
	}
	if (bFreeFill) {
		CurrentMagazineAmmo = GetMaxAmmo();
	}
	else {
		if (bRecycleAmmoOnReload) {
			int missingAmmo = GetMaxAmmo() - CurrentMagazineAmmo;
			CurrentMagazineAmmo = FMath::Clamp(GetReserveAmmo() + CurrentMagazineAmmo, 0, GetMaxAmmo());
			SetReserveAmmo(FMath::Max(0, GetReserveAmmo() - missingAmmo));
		}
		else {
			CurrentMagazineAmmo = FMath::Min(GetReserveAmmo(), GetMaxAmmo());
			SetReserveAmmo(FMath::Max(0, GetReserveAmmo() - GetMaxAmmo()));
		}
	}
	if (!bBulletChambered) {
		bBulletChambered = true;
		--CurrentMagazineAmmo;
	}
}
#pragma endregion
#pragma region Getters/Setters
int AModularFirearm::GetMaxAmmo() const {
	if (IsValid(Magazine)) {
		return Eval(Magazine->MaxAmmo);
	}
	return Eval(DefaultMaxAmmo);
}
float AModularFirearm::GetBulletSpread(int volleyCount) const {
	if (IsValid(Barrel)) {
		return Barrel->GetSpread(volleyCount, GetScalingAttribute());
	}
	return Eval(DefaultBulletSpreadForVolley, volleyCount)
		* Eval(DefaultSpreadMultiplier);
}
float AModularFirearm::GetNoise() const {
	if (IsValid(Muzzle)) {
		return Eval(Muzzle->NoiseAmount);
	}
	if (IsValid(Barrel)) {
		return Eval(Barrel->NoiseAmount);
	}
	return Eval(DefaultNoise);
}
UForceFeedbackEffect* AModularFirearm::GetHapticFeedback() const {
	if (IsValid(Grip)) {
		return Grip->HapticFeedback;
	}	
	return DefaultFiringHaptic;
}
float AModularFirearm::GetHapticIntensity() const  {
	if (IsValid(Grip)) {
		return Eval(Grip->HapticIntensity);
	}
	return Eval(DefaultFiringHapticIntensity);
}
TSubclassOf<UCameraShakeBase> AModularFirearm::GetCamShake() const  {
	if (IsValid(Stock)) {
		return Stock->CamShake;
	}
	return DefaultCamShake;
}
float AModularFirearm::GetCamShakeIntensity() const {
	if (IsValid(Grip)) {
		return Eval(Grip->CamShakeIntensity);
	}
	return Eval(DefaultCamShakeIntensity);
}
float AModularFirearm::GetFireRate() const {
	return Eval(RoundsPerSecond);
}
float AModularFirearm::GetBurstSpeed() const {
	return Eval(BurstSpeed);
}
int AModularFirearm::GetBurstAmount() const {
	return BurstAmount;
}
FTransform AModularFirearm::GetMuzzleTransform() const {
	if (IsValid(MuzzleMesh) && MuzzleMesh->DoesSocketExist(MuzzleSocketName)) {
		return MuzzleMesh->GetSocketTransform(MuzzleSocketName);
	}
	if (IsValid(BarrelMesh) && MuzzleMesh->DoesSocketExist(MuzzleSocketName)) {
		return BarrelMesh->GetSocketTransform(MuzzleSocketName);
	}
	if (IsValid(ReceiverMesh) && MuzzleMesh->DoesSocketExist(MuzzleSocketName)) {
		return ReceiverMesh->GetSocketTransform(MuzzleSocketName);
	}
	return GetActorTransform();
}
TSubclassOf<AActor> AModularFirearm::GetBulletClass() const {
	if (IsValid(Magazine)) {
		int bulletIndex = FMath::Clamp(GetScalingAttribute() - 1, 0, Magazine->BulletClasses.Num() - 1);
		if (Magazine->BulletClasses.IsValidIndex(bulletIndex)) {
			TSubclassOf<AActor> bulletClass = Magazine->BulletClasses[bulletIndex];
			if (IsValid(bulletClass)) {
				return bulletClass;
			}
		}
	}
	return DefaultBulletClass;
}
float AModularFirearm::GetReloadSpeedModifier() const {
	if (IsValid(Magazine)) {
		return Eval(Magazine->ReloadSpeedMultiplier);
	}
	return 1.0f;
}
UNiagaraSystem* AModularFirearm::GetMuzzleFlash() const {
	if (IsValid(Muzzle)) {
		return Muzzle->MuzzleFlash;
	}
	if (IsValid(Barrel)) {
		return Barrel->MuzzleFlash;
	}
	return DefaultMuzzleFlash;
}
USoundBase* AModularFirearm::GetFiringSound() const {
	if (IsValid(Muzzle)) {
		return Muzzle->GetFiringSound();
	}
	if (IsValid(Barrel)) {
		return Barrel->GetFiringSound();
	}
	return DefaultFiringSound;
}
UAnimMontage* AModularFirearm::GetReloadMontage() {
	if (IsValid(Magazine)) {
		return Magazine->GetReloadMontage();
	}
	return DefaultReloadMontage;
}
int AModularFirearm::GetReserveAmmo_Implementation() const {
	return GetMaxAmmo();
}
#pragma endregion
#pragma region Cosmetics
void AModularFirearm::PlayReplicatedMontage_Implementation(UAnimMontage* montage, const FString& info) {
	if (!IsValid(ReceiverMesh)) {
		return;
	}
	if (!IsValid(montage)) {
		if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
			bool reloadActive = animInst->Montage_IsPlaying(GetReloadMontage());
			animInst->StopAllMontages(1.f);
			if(reloadActive){
				OnReloadMontageStop.Broadcast(DefaultReloadMontage);
			}
		}
	}
	float montageSpeedModifier = 1.f;
	if (info == "Fire") {
		OnFiringMontagePlay.Broadcast(montage);
	}
	if (info == "Reload") {
		OnReloadMontagePlay.Broadcast(montage);
		montageSpeedModifier = GetReloadSpeedModifier();
	}
	if (bPlayMontagesFromExternalSource) {
		return;
	}
	if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
		animInst->Montage_Play(montage, montageSpeedModifier);
	}
}
void AModularFirearm::OnReceiverMontageEnded(UAnimMontage* Montage, bool bInterrupted) {
	if (Montage == DefaultReloadMontage) {
		bReloading = false;
		if (!bInterrupted) {
			LoadNewMagazine();
		}
	}
}
#pragma endregion
#pragma region Parent Overrides
AModularFirearm::AModularFirearm() {
	PrimaryActorTick.bCanEverTick = false;

	ReceiverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverSkeletalMesh"));
	PartMeshes.Add(ReceiverMesh);
	SetRootComponent(ReceiverMesh);
	ReceiverMesh->SetIsReplicated(true);

}
void AModularFirearm::OnConstruction(const FTransform& Transform) {
	// We do this in construction because we also support non-modular firearms.
	if (!bUseSimpleFirearm) {
		/* Create the components */ {
			BarrelMesh = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(), false));
			PartMeshes.Add(BarrelMesh);
			if (IsValid(Barrel)) {
				BarrelMesh->AttachToComponent(ReceiverMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Barrel->AttachSocketName);
			}
			BarrelMesh->SetIsReplicated(true);

			GripMesh = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(), false));
			PartMeshes.Add(GripMesh);
			if (IsValid(Grip)) {
				GripMesh->AttachToComponent(ReceiverMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Grip->AttachSocketName);
			}
			GripMesh->SetIsReplicated(true);

			MagazineMesh = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(), false));
			PartMeshes.Add(MagazineMesh);
			if (IsValid(Magazine)) {
				MagazineMesh->AttachToComponent(ReceiverMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Magazine->AttachSocketName);
			}
			MagazineMesh->SetIsReplicated(true);

			SightMesh = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(), false));
			PartMeshes.Add(SightMesh);
			if (IsValid(Sight)) {
				SightMesh->AttachToComponent(ReceiverMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Sight->AttachSocketName);
			}
			SightMesh->SetIsReplicated(true);

			StockMesh = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(), false));
			PartMeshes.Add(StockMesh);
			if (IsValid(Stock)) {
				StockMesh->AttachToComponent(ReceiverMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Stock->AttachSocketName);
			}
			StockMesh->SetIsReplicated(true);

			MuzzleMesh = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(), false));
			PartMeshes.Add(MuzzleMesh);
			if (IsValid(Muzzle)) {
				MuzzleMesh->AttachToComponent(BarrelMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Muzzle->AttachSocketName);
			}
			StockMesh->SetIsReplicated(true);
		}
		/* Generate the components */ {
			if (IsValid(Barrel) && IsValid(Barrel->Mesh)) {
				BarrelMesh->SetStaticMesh(Barrel->Mesh);
				UMaterialInterface* newMaterial = Barrel->ReceiverSkins.FindRef(DefaultSkin);
				if (IsValid(newMaterial) && IsValid(BarrelMesh)) {
					BarrelMesh->SetMaterial(0, newMaterial);
				}
			}
			if (IsValid(Grip) && IsValid(Grip->Mesh)) {
				GripMesh->SetStaticMesh(Grip->Mesh);
				UMaterialInterface* newMaterial = Grip->ReceiverSkins.FindRef(DefaultSkin);
				if (IsValid(newMaterial) && IsValid(GripMesh)) {
					GripMesh->SetMaterial(0, newMaterial);
				}
			}
			if (IsValid(Magazine) && IsValid(Magazine->Mesh)) {
				MagazineMesh->SetStaticMesh(Magazine->Mesh);
				UMaterialInterface* newMaterial = Magazine->ReceiverSkins.FindRef(DefaultSkin);
				if (IsValid(newMaterial) && IsValid(MagazineMesh)) {
					MagazineMesh->SetMaterial(0, newMaterial);
				}
			}
			if (IsValid(Sight) && IsValid(Sight->Mesh)) {
				SightMesh->SetStaticMesh(Sight->Mesh);
				UMaterialInterface* newMaterial = Sight->ReceiverSkins.FindRef(DefaultSkin);
				if (IsValid(newMaterial) && IsValid(SightMesh)) {
					SightMesh->SetMaterial(0, newMaterial);
				}
			}
			if (IsValid(Stock) && IsValid(Stock->Mesh)) {
				StockMesh->SetStaticMesh(Stock->Mesh);
				UMaterialInterface* newMaterial = Stock->ReceiverSkins.FindRef(DefaultSkin);
				if (IsValid(newMaterial) && IsValid(StockMesh)) {
					StockMesh->SetMaterial(0, newMaterial);
				}
			}
			if (IsValid(Muzzle) && IsValid(Muzzle->Mesh)) {
				MuzzleMesh->SetStaticMesh(Muzzle->Mesh);
				UMaterialInterface* newMaterial = Muzzle->ReceiverSkins.FindRef(DefaultSkin);
				if (IsValid(newMaterial) && IsValid(MuzzleMesh)) {
					MuzzleMesh->SetMaterial(0, newMaterial);
				}
			}
		}
	}
	for (int i = 0; i <= 6; i++) {
		ComponentSkins.Add(DefaultSkin);
	}

	if (bStartWithWeaponLoaded) {
		CurrentMagazineAmmo = GetMaxAmmo();
	}
}
void AModularFirearm::BeginPlay() {
	Super::BeginPlay();
	if (HasAuthority() && bStartWithWeaponLoaded) {
		bBulletChambered = true;
		CurrentMagazineAmmo = GetMaxAmmo() - 1;
	}
	if (IsValid(ReceiverMesh)) {
		if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
			animInst->OnMontageEnded.AddDynamic(this, &AModularFirearm::OnReceiverMontageEnded);
		}
	}
}
void AModularFirearm::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AModularFirearm, ComponentSkins);
	DOREPLIFETIME(AModularFirearm, bBulletChambered);
	DOREPLIFETIME(AModularFirearm, CurrentMagazineAmmo);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Muzzle, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Barrel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Grip, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Magazine, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Sight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Stock, COND_None, REPNOTIFY_Always);
}
#pragma endregion
#pragma region Customization
void AModularFirearm::SetComponent(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent) {
	if (bUseSimpleFirearm) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to use modular features with UseSimpleFirearm enabled."));
		return;
	}
	if (componentType <= EFirearmComponentType::Receiver) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to change the receiver using Gun Part Data. The receiver is not able to be changed this way."));		
		return;
	}
	if (!HasAuthority() && !GetIsReplicated()) {
		SetComponentOnServer(componentType, newComponent);
		return;
	}
	UStaticMeshComponent* modifiedComp = Cast<UStaticMeshComponent>(PartMeshes[componentType]);
	if (!IsValid(modifiedComp)) {
		UE_LOG(LogModularFirearm, Warning, TEXT("The Firearm part that was attempted to be changed is not a StaticMeshComponent."));
		return;
	}
	if (!SetPartBaseData(newComponent)) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Unable to find the part data to change."));
		return;
	}
	if (newComponent) {
		if (IsValid(newComponent->Mesh)) {
			modifiedComp->SetStaticMesh(newComponent->Mesh);
			if (IsValid(ReceiverMesh)) {
				modifiedComp->AttachToComponent(ReceiverMesh, FAttachmentTransformRules::KeepRelativeTransform, newComponent->AttachSocketName);
			}
			ReplicateSkinChange(componentType, ComponentSkins[componentType]);
		}
		else {
			modifiedComp->SetStaticMesh(nullptr);
		}
	}
}
void AModularFirearm::SetComponentSkin(const EFirearmComponentType& componentType, const FString& skinName) {
	if (!HasAuthority() && !GetIsReplicated()) {
		SetComponentSkinOnServer(componentType, skinName);
		return;
	}
	if (bUseSimpleFirearm && componentType != 0) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to use modular features with UseSimpleFirearm enabled."));
		return;
	}
	UpdateSkin(componentType, skinName);
	ComponentSkins[componentType] = skinName;
}
void AModularFirearm::ReplicateSkinChange_Implementation(const EFirearmComponentType& componentType, const FString& skinName) {
	UpdateSkin(componentType, skinName);
}
void AModularFirearm::UpdateSkin(const EFirearmComponentType& componentType, const FString& skinName) {
	UMeshComponent* modifiedMesh = PartMeshes[componentType];
	if (!IsValid(modifiedMesh)) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to update the skin of an invalid gun part."));
		return;
	}
	UMaterialInterface* newSkin;
	if (componentType == EFirearmComponentType::Receiver) {
		newSkin = ReceiverSkins.FindRef(skinName);
	}
	else {
		UGunPartDataBase* partData = GetPartData(componentType);
		if (!IsValid(partData)) {
			UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to update the skin of a gun part that doesn't have data."));
			return;
		}
		newSkin = partData->ReceiverSkins.FindRef(skinName);
	}
	if (!IsValid(newSkin)) {
		FString meshName; modifiedMesh->GetName(meshName);
		UE_LOG(LogModularFirearm, Warning, TEXT("Unable to find a skin for %s. Please check that it's data has a skin named %s"), *meshName, *skinName);
		return;
	}
	modifiedMesh->SetMaterial(0, newSkin);
}
void AModularFirearm::SetComponentOnServer_Implementation(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent) {
	if (HasAuthority()) {
		SetComponent(componentType, newComponent);
	}
}
void AModularFirearm::SetComponentSkinOnServer_Implementation(const EFirearmComponentType& componentType, const FString& skinName) {
	if (HasAuthority()) {
		SetComponentSkin(componentType, skinName);
	}
}
#pragma endregion
#pragma region Component OnRep functions
void AModularFirearm::OnRep_Muzzle() {
	if (IsValid(Muzzle) && IsValid(MuzzleMesh)) {
		if (UMaterialInterface* partMat = Muzzle->ReceiverSkins.FindRef(ComponentSkins[EFirearmComponentType::Muzzle])) {
			MuzzleMesh->SetMaterial(0, partMat);
		}
		else {
			TArray<UMaterialInterface*> mats;  Muzzle->ReceiverSkins.GenerateValueArray(mats);
			if (mats.Num() > 0) {
				MuzzleMesh->SetMaterial(0, mats[0]);
			}
		}
	}
}
void AModularFirearm::OnRep_Barrel()
{
	if(IsValid(Barrel) && IsValid(BarrelMesh)) {
		if (UMaterialInterface* partMat = Barrel->ReceiverSkins.FindRef(ComponentSkins[EFirearmComponentType::Barrel])) {
			BarrelMesh->SetMaterial(0, partMat);
		}
		else {
			TArray<UMaterialInterface*> mats;  Barrel->ReceiverSkins.GenerateValueArray(mats);
			if (mats.Num() > 0) {
				BarrelMesh->SetMaterial(0, mats[0]);
			}
		}
	}
}
void AModularFirearm::OnRep_Grip()
{
	if(IsValid(Grip) && IsValid(GripMesh)) {
		if (UMaterialInterface* partMat = Grip->ReceiverSkins.FindRef(ComponentSkins[EFirearmComponentType::Grip])) {
			GripMesh->SetMaterial(0, partMat);
		}
		else {
			TArray<UMaterialInterface*> mats;  Grip->ReceiverSkins.GenerateValueArray(mats);
			if (mats.Num() > 0) {
				GripMesh->SetMaterial(0, mats[0]);
			}
		}
	}
}
void AModularFirearm::OnRep_Magazine()
{
	if(IsValid(Magazine) && IsValid(MagazineMesh)) {
		if (UMaterialInterface* partMat = Magazine->ReceiverSkins.FindRef(ComponentSkins[EFirearmComponentType::Magazine])) {
			MagazineMesh->SetMaterial(0, partMat);
		}
		else {
			TArray<UMaterialInterface*> mats;  Magazine->ReceiverSkins.GenerateValueArray(mats);
			if (mats.Num() > 0) {
				MagazineMesh->SetMaterial(0, mats[0]);
			}
		}
	}
}
void AModularFirearm::OnRep_Sight()
{
	if (IsValid(Sight) && IsValid(SightMesh)) {
		if (UMaterialInterface* partMat = Sight->ReceiverSkins.FindRef(ComponentSkins[EFirearmComponentType::Sight])) {
			SightMesh->SetMaterial(0, partMat);
		}
		else {
			TArray<UMaterialInterface*> mats;  Sight->ReceiverSkins.GenerateValueArray(mats);
			if (mats.Num() > 0) {
				SightMesh->SetMaterial(0, mats[0]);
			}
		}
	}
}
void AModularFirearm::OnRep_Stock()
{
	if (IsValid(Stock) && IsValid(StockMesh)) {
		if (UMaterialInterface* partMat = Stock->ReceiverSkins.FindRef(ComponentSkins[EFirearmComponentType::Stock])) {
			StockMesh->SetMaterial(0, partMat);
		}
		else {
			TArray<UMaterialInterface*> mats;  Stock->ReceiverSkins.GenerateValueArray(mats);
			if (mats.Num() > 0) {
				StockMesh->SetMaterial(0, mats[0]);
			}
		}
	}
}
#pragma endregion