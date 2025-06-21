// Cutter Hodnett // 2024-


#include "Gun/ModularFirearm.h"
#include "ModularFirearmPlugin.h"
#include "Gun/ModularFirearmDataAssets.h"
#include "AIController.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gun/ModularFirearmCustomizationComponent.h"
#include "Net/UnrealNetwork.h"

#pragma region Firing
struct FStreamableManager;

void AModularFirearm::StartFiring() {
	bWantsToFire = true;
	if (GetWorld()->GetTimerManager().IsTimerActive(RecoilTimer)) {
		return;
	}
	if (FiringMode == EFiringMode::MFFM_Automatic) {
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
	if (FiringMode == EFiringMode::MFFM_Burst) {
		BurstFireWeapon(GetBurstAmount());
	}
	else {
		FireWeapon();
	}
}

void AModularFirearm::SetAllSkins_Implementation(const FString& skinName) {
	for (int i=0; i < EFirearmComponentType::MFPT_Num; i++) {
		if (i != EFirearmComponentType::MFPT_Receiver) {
			if (!bUseSimpleFirearm) {
				ReplicateSkinChange(EFirearmComponentType(i), skinName);	
			}
		}
		else {
			ReplicateSkinChange(EFirearmComponentType(i), skinName);
		}
	}
}

UGunPartDataBase* AModularFirearm::GetPartData(EFirearmComponentType type) {
	switch (type) {
	case 1: return Barrel;
	case 2: return Grip;
	case 3: return Magazine;
	case 4: return Sight;
	case 5: return Stock;
	case 6: return Muzzle;
	default: return nullptr;
	}
}

void AModularFirearm::StopFiring() {
	bWantsToFire = false;
	if (GetWorld()->GetTimerManager().TimerExists(FiringTimer)) {
		GetWorld()->GetTimerManager().PauseTimer(FiringTimer);
	}
}
void AModularFirearm::SpawnBullet_Implementation(const FVector& targetDirection) {
	TSubclassOf<AActor> bulletClass = GetBulletClass();
	if (!IsValid(bulletClass)) {
		return;
	}
	FTransform spawnTransform = FTransform(targetDirection.Rotation(),GetMuzzleTransform().GetLocation(), GetMuzzleTransform().GetScale3D());
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this; spawnParams.Instigator = GetInstigator(); spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* bullet = GetWorld()->SpawnActor<AActor>(bulletClass, spawnTransform, spawnParams);
	if (!IsValid(bullet)) {
		return;
	}
	if (CurrentMagazineAmmo == 0) {
		if(bBulletChambered){
			bBulletChambered = false;
		}
	}
	else {
		CurrentMagazineAmmo = FMath::Clamp(CurrentMagazineAmmo-1, 0, GetMaxAmmo());
	}
	OnRep_CurrentAmmo();
	OnBulletSpawn.Broadcast(bullet);
	PlayReplicatedMontage(FiringMontage);
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
			case ETargetingMode::MFTM_FocalPoint: {
				if (GetInstigator()) {
					if (GetInstigator()->IsPlayerControlled()) {
						APlayerController* playerCon = GetInstigator()->GetController<APlayerController>();
						if (APlayerCameraManager* camMan = playerCon->PlayerCameraManager) {
							FVector camLoc; FRotator camRot; camMan->GetCameraViewPoint(camLoc, camRot);
							FCollisionQueryParams hitParams; hitParams.AddIgnoredActor(GetOwner()); hitParams.AddIgnoredActor(this); hitParams.bTraceComplex = true;
							FHitResult hit;  if (GetWorld()->LineTraceSingleByChannel(hit, camLoc, camLoc + (camRot.Vector() * MAX_flt), TargetingData.TargetingChannel, hitParams)) {
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
			case ETargetingMode::MFTM_DirectionOfMuzzle: {
				FRotator rot = UKismetMathLibrary::ComposeRotators(GetMuzzleTransform().GetRotation().Rotator(), MuzzleOffset);
				targetLocation = GetMuzzleTransform().GetLocation() + rot.Vector();
				break;
			}
			case ETargetingMode::MFTM_CursorLocation: {
				APlayerController* playerCon = GetWorld()->GetFirstPlayerController();
				FHitResult hit;
				playerCon->GetHitResultUnderCursor(TargetingData.TargetingChannel, true, hit);
				if (hit.bBlockingHit) {
					targetLocation = hit.ImpactPoint;
				}
				else {
					targetLocation = hit.TraceEnd;
				}
				break;
			}
			default:{}
			}
		}
		FRandomStream stream;
		if (IsValid(GetWorld())) {
			stream = FRandomStream(GetWorld()->GetTimeSeconds());
		}
		bool bAdd = UKismetMathLibrary::RandomBoolWithWeightFromStream(stream, FMath::Fmod(GetMultishot(), 1.f));
		int additionalBullets = bAdd + GetMultishot();
		for (int i = 0; i <= additionalBullets; i++) {
			FVector targetDirection = (targetLocation - GetMuzzleTransform().GetLocation()).GetSafeNormal();
			
			targetDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegreesFromStream(stream, targetDirection, GetBulletSpread(VolleyBulletCount));
			//UKismetSystemLibrary::DrawDebugSphere(this, targetLocation)
			SpawnBullet(targetDirection);
			UKismetSystemLibrary::PrintString(this, targetLocation.ToString());
		}
		VolleyBulletCount++;
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
		float burstDelay = (1 / GetBurstSpeed());
		GetWorld()->GetTimerManager().SetTimer(burstHandle, burstDel, burstDelay, false);
	}
}
#pragma endregion
#pragma region Reloading
void AModularFirearm::Reload() {
	if (GetReserveAmmo() <= 0 || GetCurrentAmmo() >= GetMaxAmmo() || bIsReloading) {
		return;
	}
	if (GetReloadMontage()) {
		if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
			bool reloadActive = animInst->Montage_IsPlaying(GetReloadMontage());
			if (!reloadActive) {
				return;
			}
		}
		bIsReloading = true;
		OnRep_IsReloading();
		PlayReplicatedMontage(GetReloadMontage());
	}
}
void AModularFirearm::StopReloading() {
	if (bIsReloading) {
		StopReplicatedMontage(GetReloadMontage());
		bIsReloading = false;
		OnRep_IsReloading();
	}
}

void AModularFirearm::TryChangeTargetingMode(ETargetingMode newMode, bool force) {
	if (force || TargetingData.AvailableTargetingModes.Contains(newMode)) {
		TargetingMode = newMode;
	}
}

void AModularFirearm::TryChangeFiringMode(EFiringMode newMode, bool bForce) {
	if (bForce || FiringData.AvailableFiringModes.Contains(newMode)) {
		FiringMode = newMode;
	}
}

void AModularFirearm::TryIncrementFiringMode() {
	if (FiringData.AvailableFiringModes.Num() > 0) {
		for (int i = 1; i < EFiringMode::MFFM_Num; i++) {
			EFiringMode newFiringMode = EFiringMode((i + FiringMode) % EFiringMode::MFFM_Num);
			if (FiringData.AvailableFiringModes.Contains(newFiringMode)) {
				FiringMode = newFiringMode;
			}
		}
	}
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
	OnRep_CurrentAmmo();
}

FModularFirearmAmmo AModularFirearm::GetAmmo() const {
	FModularFirearmAmmo stats;
	stats.CurrentAmmo = GetCurrentAmmo();
	stats.MaxAmmo = GetMaxAmmo();
	stats.ReserveAmmo = GetReserveAmmo();
	return stats;
}

UGunPartDataBase* AModularFirearm::GetPart(EFirearmComponentType type) const {
	switch (type) {
		case EFirearmComponentType::MFPT_Barrel:
			return Barrel;
		case EFirearmComponentType::MFPT_Grip:
			return Grip;
		case EFirearmComponentType::MFPT_Magazine:
			return Magazine;
		case EFirearmComponentType::MFPT_Muzzle:
			return Muzzle;
		case EFirearmComponentType::MFPT_Sight:
			return Sight;
		case EFirearmComponentType::MFPT_Stock:
			return Stock;
	default:
		return nullptr;
	}
}

#pragma endregion
#pragma region Getters/Setters
int AModularFirearm::GetMaxAmmo_Implementation() const {
	if (Magazine) { return Magazine->GetCapacity(); }
	return DefaultMaxAmmo;
}
float AModularFirearm::GetBulletSpread_Implementation(int volleyCount) const {
	float retVal = 0.f;
	if (DefaultVolleySpread) { retVal = DefaultVolleySpread->GetFloatValue(volleyCount); }
	if (Barrel) { retVal = Barrel->GetVolleySpread(volleyCount); }
	if (Muzzle) { retVal = Muzzle->GetVolleySpread(volleyCount); }
	return retVal;
}
float AModularFirearm::GetNoise_Implementation() const {
	if (Barrel) { return Barrel->Noise; }
	if (Muzzle) { return Muzzle->Noise; }
	return DefaultNoise;
}
UForceFeedbackEffect* AModularFirearm::GetHapticFeedback_Implementation() const {
	if (IsValid(Grip)) { return Grip->HapticFeedback; }	
	return DefaultFiringHaptic;
}
float AModularFirearm::GetHapticIntensity_Implementation() const  {
	if (Grip) { return Grip->HapticIntensity; }
	return DefaultFiringHapticIntensity;
}
TSubclassOf<UCameraShakeBase> AModularFirearm::GetCamShake_Implementation() const  {
	if (IsValid(Stock)) { return Stock->CamShake; }
	return DefaultCamShake;
}
float AModularFirearm::GetCamShakeIntensity_Implementation() const {
	if (Stock) { return Stock->CamShakeIntensity; }
	return DefaultCamShakeIntensity;
}
float AModularFirearm::GetFireRate_Implementation() const {
	return FiringData.FiringRate;
}
float AModularFirearm::GetMultishot_Implementation() const {
	return FiringData.Multishot;
}
float AModularFirearm::GetBurstSpeed_Implementation() const {
	return FiringData.BurstSpeed;
}
int AModularFirearm::GetBurstAmount_Implementation() const {
	return FiringData.BurstCount;
}
FTransform AModularFirearm::GetMuzzleTransform_Implementation() const {
	if (IsValid(MuzzleMesh) && IsValid(MuzzleMesh->GetStaticMesh()) && MuzzleMesh->DoesSocketExist(BulletSpawningSocket)) {
		return MuzzleMesh->GetSocketTransform(BulletSpawningSocket);
	}
	if (IsValid(BarrelMesh) && IsValid(BarrelMesh->GetStaticMesh()) && BarrelMesh->DoesSocketExist(BulletSpawningSocket)) {
		return BarrelMesh->GetSocketTransform(BulletSpawningSocket);
	}
	if (IsValid(ReceiverMesh) && ReceiverMesh->DoesSocketExist(BulletSpawningSocket)) {
		return ReceiverMesh->GetSocketTransform(BulletSpawningSocket);
	}
	return GetActorTransform();
}
TSubclassOf<AActor> AModularFirearm::GetBulletClass_Implementation(int bulletType) const {
	if (IsValid(Magazine)) {
		int bulletIndex = FMath::Clamp(bulletType, 0, Magazine->BulletClasses.Num() - 1);
		if (Magazine->BulletClasses.IsValidIndex(bulletIndex)) {
			TSubclassOf<AActor> bulletClass = Magazine->BulletClasses[bulletIndex];
			if (IsValid(bulletClass)) {
				return bulletClass;
			}
		}
	}
	return DefaultBulletClass;
}
float AModularFirearm::GetReloadSpeedModifier_Implementation() const {
	if (Magazine) { return Magazine->ReloadSpeed; }
	return 1.0;
}
TSoftObjectPtr<UNiagaraSystem> AModularFirearm::GetMuzzleFlash_Implementation() const {
	if (IsValid(Muzzle)) { return Muzzle->MuzzleFlash; }
	if (IsValid(Barrel)) { return Barrel->MuzzleFlash; }
	return DefaultMuzzleFlash;
}
TSoftObjectPtr<USoundBase> AModularFirearm::GetFiringSound_Implementation() const {
	if (IsValid(Muzzle)) { return Muzzle->FiringSound; }
	if (IsValid(Barrel)) { return Barrel->FiringSound; }
	return DefaultFiringSound;
}
UAnimMontage* AModularFirearm::GetReloadMontage_Implementation() {
	if (IsValid(Magazine)) { return Magazine->GetReloadMontage(); }
	return DefaultReloadMontage;
}
int AModularFirearm::GetReserveAmmo_Implementation() const {
	return GetMaxAmmo();
}
#pragma endregion
#pragma region Cosmetics
void AModularFirearm::PlayReplicatedMontage_Implementation(UAnimMontage* montage) {
	if (!IsValid(ReceiverMesh)) {
		return;
	}

	float montageSpeedModifier = 1.f;
	if (GetReloadMontage() == montage) {
		montageSpeedModifier = GetReloadSpeedModifier();
	}
	
	if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
		animInst->Montage_Play(montage, montageSpeedModifier);
	}
}

void AModularFirearm::StopReplicatedMontage_Implementation(UAnimMontage* montage) {
	if (montage) {
		if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
			bool reloadActive = animInst->Montage_IsPlaying(GetReloadMontage());
			animInst->Montage_Stop(.1f, montage);
		}
	}
}

void AModularFirearm::OnReceiverMontageEnded(UAnimMontage* Montage, bool bInterrupted) {
	if (Montage == DefaultReloadMontage) {
		bIsReloading = false;
		if (HasAuthority()) {
			OnRep_IsReloading();
		}
		if (!bInterrupted) {
			LoadNewMagazine();
		}
	}
}
#pragma endregion
#pragma region Parent Overrides
AModularFirearm::AModularFirearm() {
	PrimaryActorTick.bCanEverTick = false;

	CustomizationComponent = CreateDefaultSubobject<UModularFirearmCustomizationComponent>(TEXT("CustomizationComponent"));
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
				UMeshComponent* mesh = ReceiverMesh;
				if (Barrel) {mesh = BarrelMesh;}
				MuzzleMesh->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Muzzle->AttachSocketName);
				
			}
			MuzzleMesh->SetIsReplicated(true);
		}
		if (CustomizationComponent) {
			FFirearmMeshComponents comps;
				comps.Receiver = ReceiverMesh;
				comps.Barrel = BarrelMesh;
				comps.Grip = GripMesh;
				comps.Magazine = MagazineMesh;
				comps.Sight = SightMesh;
				comps.Stock = StockMesh;
				comps.Muzzle = MuzzleMesh;
			CustomizationComponent->SetMeshComponents(comps);
			
			if (BarrelMesh && Barrel)		{ BarrelMesh->SetStaticMesh(	Barrel->Mesh.Get()		); } else { BarrelMesh->SetStaticMesh(nullptr);		}
			if (GripMesh && Grip)			{ GripMesh->SetStaticMesh(		Grip->Mesh.Get()		); } else { GripMesh->SetStaticMesh(nullptr);		}
			if (MagazineMesh && Magazine)	{ MagazineMesh->SetStaticMesh(	Magazine->Mesh.Get()	); } else { MagazineMesh->SetStaticMesh(nullptr);	}
			if (SightMesh && Sight)			{ SightMesh->SetStaticMesh(		Sight->Mesh.Get()		); } else { SightMesh->SetStaticMesh(nullptr);		}
			if (StockMesh && Stock)			{ StockMesh->SetStaticMesh(		Stock->Mesh.Get()		); } else { StockMesh->SetStaticMesh(nullptr);		}
			if (MuzzleMesh && Muzzle)		{ MuzzleMesh->SetStaticMesh(	Muzzle->Mesh.Get()		); } else { MuzzleMesh->SetStaticMesh(nullptr);		}
			
		}
	}
	if  (bStartWithWeaponLoaded)	{	CurrentMagazineAmmo = GetMaxAmmo();		}
}
void AModularFirearm::BeginPlay() {
	Super::BeginPlay();
	TargetingMode = TargetingData.DefaultTargetingMode;
	FiringMode = FiringData.DefaultFiringMode;
	if (IsValid(ReceiverMesh)) {
		if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
			animInst->OnMontageEnded.AddDynamic(this, &AModularFirearm::OnReceiverMontageEnded);
		}		
	}
	if (HasAuthority() && bStartWithWeaponLoaded) {
		bBulletChambered = true;
		CurrentMagazineAmmo = GetMaxAmmo() - 1;
		OnRep_CurrentAmmo();
	}
}

inline void AModularFirearm::PostLoad() {
	Super::PostLoad();
	FStreamableManager& streamableManager = UAssetManager::GetStreamableManager();
	
	if (Barrel && BarrelMesh ) {
		streamableManager.RequestSyncLoad(Barrel->Mesh.ToSoftObjectPath());
		BarrelMesh->SetStaticMesh(Barrel->Mesh.LoadSynchronous());
	}
	if (Grip && GripMesh) {
		streamableManager.RequestSyncLoad(Grip->Mesh.ToSoftObjectPath());
		GripMesh->SetStaticMesh(Grip->Mesh.LoadSynchronous());
	}
	if (Magazine && MagazineMesh) {
		streamableManager.RequestSyncLoad(Magazine->Mesh.ToSoftObjectPath());
		MagazineMesh->SetStaticMesh(Magazine->Mesh.LoadSynchronous());
	}
	if (Sight && SightMesh) {
		streamableManager.RequestSyncLoad(Sight->Mesh.ToSoftObjectPath());
		SightMesh->SetStaticMesh(Sight->Mesh.LoadSynchronous());
	}
	if (Stock && StockMesh) {
		streamableManager.RequestSyncLoad(Stock->Mesh.ToSoftObjectPath());
		StockMesh->SetStaticMesh(Stock->Mesh.LoadSynchronous());
	}
	if (Muzzle && MuzzleMesh) {
		streamableManager.RequestSyncLoad(Muzzle->Mesh.ToSoftObjectPath());
		MuzzleMesh->SetStaticMesh(Muzzle->Mesh.LoadSynchronous());
	}
}
void AModularFirearm::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AModularFirearm, bBulletChambered);
	DOREPLIFETIME(AModularFirearm, CurrentMagazineAmmo);
	
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Muzzle, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Barrel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Grip, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Magazine, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Sight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Stock, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, bIsFiring, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, bIsReloading, COND_None, REPNOTIFY_Always);
}
#pragma endregion
#pragma region Customization
void AModularFirearm::SetModularPart_Implementation(EFirearmComponentType componentType, UGunPartDataBase* part) {
	if (bUseSimpleFirearm) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to use modular features with UseSimpleFirearm enabled."));
		return;
	}
	if (componentType <= EFirearmComponentType::MFPT_Receiver) {
		UE_LOG(LogModularFirearm, Warning, TEXT("Attempted to change the receiver using Gun Part Data. The receiver is not able to be changed this way."));		
		return;
	}
	switch (componentType) {
	case MFPT_Muzzle: { Muzzle = Cast<UGunMuzzleData>(part);		OnRep_Muzzle();		break;}
	case MFPT_Barrel:{ Barrel = Cast<UGunBarrelData>(part);			OnRep_Barrel();		break;}
	case MFPT_Grip:{ Grip = Cast<UGunGripData>(part);				OnRep_Grip();		break;}
	case MFPT_Magazine:{ Magazine = Cast<UGunMagazineData>(part);	OnRep_Magazine();	break;}
	case MFPT_Sight:{ Sight = Cast<UGunSightData>(part);			OnRep_Sight();		break;}
	case MFPT_Stock:{ Stock = Cast<UGunStockData>(part);			OnRep_Stock();		break;}
	default: break;
	}
}
void AModularFirearm::SetModularPartSkin_Implementation(EFirearmComponentType componentType, const FString& skinName) {
	if (componentType == EFirearmComponentType::MFPT_Num) { return; }
	if (bUseSimpleFirearm && componentType != EFirearmComponentType::MFPT_Receiver) { return;}
	ReplicateSkinChange(componentType, skinName);
}
void AModularFirearm::ReplicateSkinChange_Implementation(const EFirearmComponentType& componentType, const FString& skinName) {
	CustomizationComponent->ChangeSkin(componentType, skinName);
	OnSkinChanged.Broadcast(componentType, skinName);
}
#pragma endregion
#pragma region Component OnRep functions
void AModularFirearm::OnRep_Muzzle() {
	CustomizationComponent->ChangePart(EFirearmComponentType::MFPT_Muzzle, Muzzle);
	OnPartChanged.Broadcast(EFirearmComponentType::MFPT_Muzzle, Muzzle);
}
void AModularFirearm::OnRep_Barrel() {
	CustomizationComponent->ChangePart(EFirearmComponentType::MFPT_Barrel, Barrel);
	OnPartChanged.Broadcast(EFirearmComponentType::MFPT_Barrel, Barrel);
}
void AModularFirearm::OnRep_Grip() {
	CustomizationComponent->ChangePart(EFirearmComponentType::MFPT_Grip, Grip);
	OnPartChanged.Broadcast(EFirearmComponentType::MFPT_Grip, Grip);
}
void AModularFirearm::OnRep_Magazine() {
	CustomizationComponent->ChangePart(EFirearmComponentType::MFPT_Magazine, Magazine);
	OnPartChanged.Broadcast(EFirearmComponentType::MFPT_Magazine, Magazine);
}
void AModularFirearm::OnRep_Sight() {
	CustomizationComponent->ChangePart(EFirearmComponentType::MFPT_Sight, Sight);
	OnPartChanged.Broadcast(EFirearmComponentType::MFPT_Sight, Sight);
}
void AModularFirearm::OnRep_Stock() {
	CustomizationComponent->ChangePart(EFirearmComponentType::MFPT_Stock, Stock);
	OnPartChanged.Broadcast(EFirearmComponentType::MFPT_Stock, Stock);
}

void AModularFirearm::OnRep_IsFiring() {
	if (bIsFiring) {
		OnBeginFiring.Broadcast();
	}
	else {
		OnEndFiring.Broadcast();
	}
}

void AModularFirearm::OnRep_IsReloading() {
	if (bIsReloading) {
		OnBeginReloading.Broadcast();
	}
	else {
		OnEndReloading.Broadcast();
	}
}
#pragma endregion
