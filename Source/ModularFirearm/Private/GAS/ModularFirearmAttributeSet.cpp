// Cutter Hodnett // 2024-


#include "GAS/ModularFirearmAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UModularFirearmAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, SpreadMultiplier, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, Noise, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, CamShakeIntensity, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, HapticIntensity, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, ReloadSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, FOVZoom, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, SwapSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, Multishot, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, FireRate, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, BurstSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UModularFirearmAttributeSet, BurstAmount, COND_None, REPNOTIFY_Always);
}
