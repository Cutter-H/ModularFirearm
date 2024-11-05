// Cutter Hodnett // 2022-


#include "Gun/Gun.h"
#include "Net/UnrealNetwork.h"

#include "ModularFirearmDataAssets.h"
#include "Gun/GunComponent.h"
#include "Gun/GunAttachment.h"
#include "Gun/GunBarrel.h"
#include "Gun/GunGrip.h"
#include "Gun/GunMagazine.h"
#include "Gun/GunSight.h"
#include "Gun/GunStock.h"

// Sets default values
AModularFirearm::AModularFirearm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ReceiverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReceiverSkeletalMesh"));
	SetRootComponent(ReceiverMesh);

	AttachmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachmentMesh"));
	AttachmentMesh->SetupAttachment(ReceiverMesh, AttachmentBoneName);

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetupAttachment(ReceiverMesh, BarrelBoneName);

	GripMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GripMesh"));
	GripMesh->SetupAttachment(ReceiverMesh, GripBoneName);

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(ReceiverMesh, MagazineBoneName);

	SightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SightMesh"));
	SightMesh->SetupAttachment(ReceiverMesh, SightBoneName);

	StockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StockMesh"));
	StockMesh->SetupAttachment(ReceiverMesh, StockBoneName);
	if (DefaultParts)
	{
		if(DefaultParts->Attachment)
			if (DefaultParts->Stock->StaticMesh)
				AttachmentMesh->SetStaticMesh(DefaultParts->Attachment->StaticMesh);
		if (DefaultParts->Barrel)
			if (DefaultParts->Barrel->StaticMesh)
				BarrelMesh->SetStaticMesh(DefaultParts->Barrel->StaticMesh);
		if (DefaultParts->Grip)
			if (DefaultParts->Grip->StaticMesh)
				GripMesh->SetStaticMesh(DefaultParts->Grip->StaticMesh);
		if (DefaultParts->Magazine)
			if (DefaultParts->Magazine->StaticMesh)
				MagazineMesh->SetStaticMesh(DefaultParts->Magazine->StaticMesh);
		if (DefaultParts->Sight)
			if (DefaultParts->Sight->StaticMesh)
				SightMesh->SetStaticMesh(DefaultParts->Sight->StaticMesh);
		if (DefaultParts->Stock)
			if(DefaultParts->Stock->StaticMesh)
				StockMesh->SetStaticMesh(DefaultParts->Stock->StaticMesh);
	}
}

// Called when the game starts or when spawned
void AModularFirearm::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AModularFirearm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AModularFirearm::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AModularFirearm, Attachment);
	DOREPLIFETIME(AModularFirearm, Barrel);
	DOREPLIFETIME(AModularFirearm, Grip);
	DOREPLIFETIME(AModularFirearm, Magazine);
	DOREPLIFETIME(AModularFirearm, Sight);
	DOREPLIFETIME(AModularFirearm, Stock);
}

// Component On Rep functions

void AModularFirearm::OnRep_Attachment()
{
	if (Attachment)
	{
		AttachmentMesh->SetStaticMesh(Attachment->GetMesh());
	}
}

void AModularFirearm::OnRep_Barrel()
{
	BarrelMesh->SetStaticMesh(Barrel->GetMesh());
}

void AModularFirearm::OnRep_Grip()
{
	GripMesh->SetStaticMesh(Grip->GetMesh());
}

void AModularFirearm::OnRep_Magazine()
{
	MagazineMesh->SetStaticMesh(Magazine->GetMesh());
}

void AModularFirearm::OnRep_Sight()
{
	SightMesh->SetStaticMesh(Sight->GetMesh());
}

void AModularFirearm::OnRep_Stock()
{
	StockMesh->SetStaticMesh(Stock->GetMesh());
}