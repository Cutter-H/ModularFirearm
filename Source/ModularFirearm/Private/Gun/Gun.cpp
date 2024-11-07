// Cutter Hodnett // 2024


#include "Gun/Gun.h"
#include "AIController.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#pragma region Firing
void AModularFirearm::BeginFiring() {
	bWantsToFire = true;
	if (FiringTimer.IsValid()) {
		return;
	}
	if (bContinuousFire) {
		float delay = 1 / GetFireRate();
		GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &AModularFirearm::FireWeapon, delay, true);
	}
	FireWeapon();
}
void AModularFirearm::StopFiring() {
	bWantsToFire = false;
}
void AModularFirearm::SpawnBullet_Implementation(const FVector& targetLocation) {
	TSubclassOf<AActor> bulletClass = GetBulletClass();
	if (!IsValid(bulletClass)) {
		return;
	}
	FTransform spawnTransform = GetMuzzleTransform();
	FVector direction = UKismetMathLibrary::FindLookAtRotation(spawnTransform.GetLocation(), targetLocation).Vector();
	direction = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(direction, GetBulletSpread());
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
		bBulletLoaded = false;
	}
}
void AModularFirearm::OnRep_CurrentAmmo() {
	OnCurrentAmmoChange.Broadcast(CurrentMagazineAmmo + bBulletLoaded);
}
void AModularFirearm::FireWeapon() {
	if (!bWantsToFire) {
		FiringTimer.Invalidate();
		GetWorld()->GetTimerManager().ClearTimer(FiringTimer);
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
				targetLocation = GetMuzzleTransform().GetLocation() + GetMuzzleTransform().GetRotation().GetForwardVector();
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
				break;
			}
			}
		}
		SpawnBullet(targetLocation);
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
			UGameplayStatics::SpawnForceFeedbackAttached(GetHapticFeedback(), ReceiverMesh, GripBoneName, FVector(), FRotator(), EAttachLocation::SnapToTarget, true, false, GetHapticIntensity());
		}
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
		bool reloadActive = animInst->Montage_IsPlaying(ReloadMontage);
		if (!reloadActive) {
			return;
		}
	}
	PlayReplicatedMontage(ReloadMontage, "Reload");
}
void AModularFirearm::LoadNewMagazine(bool bFreeFill) {
	if (!HasAuthority() || GetReserveAmmo() <= 0) {
		return;
	}
	if (bFreeFill) {
		CurrentMagazineAmmo = GetMaxAmmo();
	}
	else {
		int missingAmmo = GetMaxAmmo() - CurrentMagazineAmmo;
		if (bRecycleAmmoOnReload) {
			CurrentMagazineAmmo = FMath::Clamp(GetReserveAmmo() + CurrentMagazineAmmo, 0, GetMaxAmmo());
			SetReserveAmmo(FMath::Max(0, GetReserveAmmo() - missingAmmo));
		}
		else {
			CurrentMagazineAmmo = FMath::Min(GetReserveAmmo(), GetMaxAmmo());
			SetReserveAmmo(FMath::Max(0, GetReserveAmmo() - GetMaxAmmo()));
		}
	}
	if (!bBulletLoaded) {
		bBulletLoaded = true;
		--CurrentMagazineAmmo;
	}
}
#pragma endregion
#pragma region Getters/Setters
int AModularFirearm::GetMaxAmmo() const {
	if (IsValid(Magazine)) {
		return Magazine->MaxAmmo.GetValue(FirearmLevel);
	}
	return DefaultMaxAmmo.GetValue(FirearmLevel);
}
float AModularFirearm::GetBulletSpread() const {
	if (IsValid(Barrel)) {
		return Barrel->bulletSpreadDegree.GetValue(FirearmLevel);
	}
	return 0.f;
}
float AModularFirearm::GetNoise() const {
	if (IsValid(Barrel)) {
		return Barrel->NoiseAmount.GetValue(FirearmLevel);
	}
	return DefaultNoise.GetValue(FirearmLevel);
}
UForceFeedbackEffect* AModularFirearm::GetHapticFeedback() const {
	if (IsValid(Grip)) {
		return Grip->HapticFeedback;
	}	
	return DefaultFiringHaptic;
}
float AModularFirearm::GetHapticIntensity() const  {
	if (IsValid(Grip)) {
		return Grip->HapticIntensity.GetValue(FirearmLevel);
	}
	return 1.f;
}
TSubclassOf<UCameraShakeBase> AModularFirearm::GetCamShake() const  {
	if (IsValid(Stock)) {
		return Stock->CamShake;
	}
	return DefaultCamShake;
}
float AModularFirearm::GetCamShakeIntensity() const {
	if (IsValid(Grip)) {
		return Grip->CamShakeIntensity.GetValue(FirearmLevel);
	}
	return 1.0f;
}
float AModularFirearm::GetFireRate() const {
	return RoundsPerSecond.GetValue(FirearmLevel);
}
FTransform AModularFirearm::GetMuzzleTransform() const {
	if (IsValid(BarrelMesh)) {
		return BarrelMesh->GetSocketTransform(MuzzleSocketName);
	}
	if (IsValid(ReceiverMesh)) {
		return ReceiverMesh->GetSocketTransform(MuzzleSocketName);
	}
	return GetActorTransform();
}
TSubclassOf<AActor> AModularFirearm::GetBulletClass() const {
	if (IsValid(Magazine)) {
		return Magazine->BulletClass;
	}
	return TSubclassOf<AActor>();
}
float AModularFirearm::GetReloadSpeedModifier() const {
	if (IsValid(Magazine)) {
		return Magazine->ReloadSpeedMultiplier.GetValue(FirearmLevel);
	}
	return 1.0f;
}
int AModularFirearm::GetReserveAmmo_Implementation() const {
	return GetMaxAmmo();
}
#pragma endregion
void AModularFirearm::PlayReplicatedMontage_Implementation(UAnimMontage* montage, const FString& info) {
	if (!IsValid(ReceiverMesh)) {
		return;
	}
	if (!IsValid(montage)) {
		if (UAnimInstance* animInst = ReceiverMesh->GetAnimInstance()) {
			bool reloadActive = animInst->Montage_IsPlaying(ReloadMontage);
			animInst->StopAllMontages(1.f);
			if(reloadActive){
				OnReloadMontageStop.Broadcast(ReloadMontage);
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
	if (Montage == ReloadMontage) {
		bReloading = false;
		if (!bInterrupted) {
			LoadNewMagazine();
		}
	}
}
#pragma region Overrides
AModularFirearm::AModularFirearm() {
	PrimaryActorTick.bCanEverTick = false;

	ReceiverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverSkeletalMesh"));
	SetRootComponent(ReceiverMesh);
	ReceiverMesh->SetIsReplicated(true);

	AttachmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AttachmentMesh"));
	AttachmentMesh->SetupAttachment(ReceiverMesh, AttachmentBoneName);
	AttachmentMesh->SetIsReplicated(true);

	BarrelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetupAttachment(ReceiverMesh, BarrelBoneName);
	BarrelMesh->SetIsReplicated(true);

	GripMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GripMesh"));
	GripMesh->SetupAttachment(ReceiverMesh, GripBoneName);
	GripMesh->SetIsReplicated(true);

	MagazineMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(ReceiverMesh, MagazineBoneName);
	MagazineMesh->SetIsReplicated(true);

	SightMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SightMesh"));
	SightMesh->SetupAttachment(ReceiverMesh, SightBoneName);
	SightMesh->SetIsReplicated(true);

	StockMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StockMesh"));
	StockMesh->SetupAttachment(ReceiverMesh, StockBoneName);
	StockMesh->SetIsReplicated(true);

}
void AModularFirearm::OnConstruction(const FTransform& Transform) {
	if (DefaultParts) {
		if (DefaultParts->Attachment)
			if (DefaultParts->Stock->Mesh)
				AttachmentMesh->SetSkeletalMesh(DefaultParts->Attachment->Mesh);
		if (DefaultParts->Barrel)
			if (DefaultParts->Barrel->Mesh)
				BarrelMesh->SetSkeletalMesh(DefaultParts->Barrel->Mesh);
		if (DefaultParts->Grip)
			if (DefaultParts->Grip->Mesh)
				GripMesh->SetSkeletalMesh(DefaultParts->Grip->Mesh);
		if (DefaultParts->Magazine)
			if (DefaultParts->Magazine->Mesh)
				MagazineMesh->SetSkeletalMesh(DefaultParts->Magazine->Mesh);
		if (DefaultParts->Sight)
			if (DefaultParts->Sight->Mesh)
				SightMesh->SetSkeletalMesh(DefaultParts->Sight->Mesh);
		if (DefaultParts->Stock)
			if (DefaultParts->Stock->Mesh)
				StockMesh->SetSkeletalMesh(DefaultParts->Stock->Mesh);
	}
	FString skinName = DefaultSkin;
	if (IsValid(DefaultParts)) {
		skinName = DefaultParts->DefaultSkin;
	}
	if (Attachment) {
		UMaterialInterface* newMaterial = Attachment->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(AttachmentMesh)) {
			AttachmentMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Barrel) {
		UMaterialInterface* newMaterial = Barrel->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(BarrelMesh)) {
			BarrelMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Grip) {
		UMaterialInterface* newMaterial = Grip->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(GripMesh)) {
			GripMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Magazine) {
		UMaterialInterface* newMaterial = Magazine->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(MagazineMesh)) {
			MagazineMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Sight) {
		UMaterialInterface* newMaterial = Sight->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(SightMesh)) {
			SightMesh->SetMaterial(0, newMaterial);
		}
	}
	if (Stock) {
		UMaterialInterface* newMaterial = Stock->Skins.FindRef(skinName);
		if (IsValid(newMaterial) && IsValid(StockMesh)) {
			StockMesh->SetMaterial(0, newMaterial);
		}
	}
	for (int i = 0; i <= 5; i++) {
		ComponentSkins.Add(skinName);
	}

}
void AModularFirearm::BeginPlay() {
	Super::BeginPlay();
	if (HasAuthority() && bStartWithWeaponLoaded) {
		bBulletLoaded = true;
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
	DOREPLIFETIME(AModularFirearm, bBulletLoaded);
	DOREPLIFETIME(AModularFirearm, CurrentMagazineAmmo);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Attachment, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Barrel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Grip, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Magazine, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Sight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AModularFirearm, Stock, COND_None, REPNOTIFY_Always);
}
#pragma endregion
#pragma region Customization
void AModularFirearm::SetComponent(const EFirearmComponentType& componentType, UGunPartDataBase* newComponent) {
	if (componentType == EFirearmComponentType::Receiver) {
		return;
	}
	if (!HasAuthority() && !GetIsReplicated()) {
		SetComponentOnServer(componentType, newComponent);
		return;
	}
	switch (componentType) {
	case EFirearmComponentType::Attachment: {
		Attachment = Cast<UGunAttachmentData>(newComponent);
		if (Attachment) {
			if (Attachment->Mesh) {
				AttachmentMesh->SetSkeletalMesh(Attachment->Mesh);
				if (IsValid(Attachment->DefaultAnimInstance)) {
					AttachmentMesh->SetAnimInstanceClass(Attachment->DefaultAnimInstance);
				}
				break;
			}
		}
		AttachmentMesh->SetSkeletalMesh(nullptr);
		break;
	}
	case EFirearmComponentType::Barrel: {
		Barrel = Cast<UGunBarrelData>(newComponent);
		if (Barrel) {
			if (Barrel->Mesh) {
				BarrelMesh->SetSkeletalMesh(Barrel->Mesh);
				if (IsValid(Barrel->DefaultAnimInstance)) {
					BarrelMesh->SetAnimInstanceClass(Barrel->DefaultAnimInstance);
				}
				break;
			}
		}
		BarrelMesh->SetSkeletalMesh(nullptr);
		break;
	}
	case EFirearmComponentType::Grip: {
		Grip = Cast<UGunGripData>(newComponent);
		if (Grip) {
			if (Grip->Mesh) {
				GripMesh->SetSkeletalMesh(Grip->Mesh);
				if (IsValid(Grip->DefaultAnimInstance)) {
					GripMesh->SetAnimInstanceClass(Grip->DefaultAnimInstance);
				}
				break;
			}
		}
		GripMesh->SetSkeletalMesh(nullptr);
		break;
	}
	case EFirearmComponentType::Magazine: {
		Magazine = Cast<UGunMagazineData>(newComponent);
		if (Magazine) {
			if (Magazine->Mesh) {
				MagazineMesh->SetSkeletalMesh(Magazine->Mesh);
				if (IsValid(Magazine->DefaultAnimInstance)) {
					MagazineMesh->SetAnimInstanceClass(Magazine->DefaultAnimInstance);
				}
				break;
			}
		}
		MagazineMesh->SetSkeletalMesh(nullptr);
		break;
	}
	case EFirearmComponentType::Sight: {
		Sight = Cast<UGunSightData>(newComponent);
		if (Sight) {
			if (Sight->Mesh) {
				SightMesh->SetSkeletalMesh(Sight->Mesh);
				if (IsValid(Sight->DefaultAnimInstance)) {
					SightMesh->SetAnimInstanceClass(Sight->DefaultAnimInstance);
				}
				break;
			}
		}
		SightMesh->SetSkeletalMesh(nullptr);
		break;
	}
	case EFirearmComponentType::Stock: {
		Stock = Cast<UGunStockData>(newComponent);
		if (Stock) {
			if (Stock->Mesh) {
				StockMesh->SetSkeletalMesh(Stock->Mesh);
				if (IsValid(Stock->DefaultAnimInstance)) {
					StockMesh->SetAnimInstanceClass(Stock->DefaultAnimInstance);
				}
				break;
			}
		}
		StockMesh->SetSkeletalMesh(nullptr);
		break;
	}
	}
	ReplicateSkinChange(componentType, ComponentSkins[componentType]);
}
void AModularFirearm::SetComponentSkin(const EFirearmComponentType& componentType, const FString& skinName) {
	if (!HasAuthority() && !GetIsReplicated()) {
		SetComponentSkinOnServer(componentType, skinName);
		return;
	}
	UpdateSkin(componentType, skinName);
	ComponentSkins[componentType] = skinName;
}
void AModularFirearm::ReplicateSkinChange_Implementation(const EFirearmComponentType& componentType, const FString& skinName) {
	UpdateSkin(componentType, skinName);
}
void AModularFirearm::OnRep_FirearmLevel() {

}
void AModularFirearm::UpdateSkin(const EFirearmComponentType& componentType, const FString& skinName) {
	switch (componentType) {
	case EFirearmComponentType::Receiver: {
		if (IsValid(ReceiverMesh)) {
			if (UMaterialInterface* newMaterial = Skins.FindRef(skinName)) {
				ReceiverMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	case EFirearmComponentType::Attachment: {
		if (IsValid(AttachmentMesh) && IsValid(Attachment)) {
			if (UMaterialInterface* newMaterial = Attachment->Skins.FindRef(skinName)) {
				AttachmentMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	case EFirearmComponentType::Barrel: {
		if (IsValid(BarrelMesh) && IsValid(Barrel)) {
			if (UMaterialInterface* newMaterial = Barrel->Skins.FindRef(skinName)) {
				BarrelMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	case EFirearmComponentType::Grip: {
		if (IsValid(GripMesh) && IsValid(Grip)) {
			if (UMaterialInterface* newMaterial = Grip->Skins.FindRef(skinName)) {
				GripMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	case EFirearmComponentType::Magazine: {
		if (IsValid(MagazineMesh) && IsValid(Magazine)) {
			if (UMaterialInterface* newMaterial = Magazine->Skins.FindRef(skinName)) {
				MagazineMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	case EFirearmComponentType::Sight: {
		if (IsValid(SightMesh) && IsValid(Sight)) {
			if (UMaterialInterface* newMaterial = Sight->Skins.FindRef(skinName)) {
				SightMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	case EFirearmComponentType::Stock: {
		if (IsValid(StockMesh) && IsValid(Stock)) {
			if (UMaterialInterface* newMaterial = Stock->Skins.FindRef(skinName)) {
				StockMesh->SetMaterial(0, newMaterial);
			}
		}
		break;
	}
	}
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
#pragma region Component On Rep functions
void AModularFirearm::OnRep_Attachment()
{
	if (IsValid(Attachment) && IsValid(AttachmentMesh))
	{
		AttachmentMesh->SetSkeletalMesh(Attachment->Mesh);
		if (IsValid(Attachment->DefaultAnimInstance)) {
			AttachmentMesh->SetAnimInstanceClass(Attachment->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Barrel()
{
	if(IsValid(Barrel) && IsValid(BarrelMesh)) {
		BarrelMesh->SetSkeletalMesh(Barrel->Mesh);
		if (IsValid(Barrel->DefaultAnimInstance)) {
			BarrelMesh->SetAnimInstanceClass(Barrel->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Grip()
{
	if(IsValid(Grip) && IsValid(GripMesh)) {
		GripMesh->SetSkeletalMesh(Grip->Mesh);
		if (IsValid(Grip->DefaultAnimInstance)) {
			GripMesh->SetAnimInstanceClass(Grip->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Magazine()
{
	if(IsValid(Magazine) && IsValid(MagazineMesh)) {
		MagazineMesh->SetSkeletalMesh(Magazine->Mesh);
		if (IsValid(Magazine->DefaultAnimInstance)) {
			MagazineMesh->SetAnimInstanceClass(Magazine->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Sight()
{
	if (IsValid(Sight) && IsValid(SightMesh)) {
		SightMesh->SetSkeletalMesh(Sight->Mesh);
		if (IsValid(Sight->DefaultAnimInstance)) {
			SightMesh->SetAnimInstanceClass(Sight->DefaultAnimInstance);
		}
	}
}
void AModularFirearm::OnRep_Stock()
{
	if (IsValid(Stock) && IsValid(StockMesh)) {
		StockMesh->SetSkeletalMesh(Stock->Mesh);
		if (IsValid(Stock->DefaultAnimInstance)) {
			StockMesh->SetAnimInstanceClass(Stock->DefaultAnimInstance);
		}
	}
}
#pragma endregion