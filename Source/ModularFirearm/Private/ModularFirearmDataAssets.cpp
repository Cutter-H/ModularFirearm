// Cutter Hodnett // 2024-


#include "ModularFirearmDataAssets.h"
#include "Engine/AssetManager.h"
#include "GAS/ModularFirearmAttributeSet.h"

UGameplayEffect* UGunPartDataBase::MakeEffect(UModularFirearmAttributeSet* attributes) const {
    return nullptr;
}

void UGunPartDataBase::LoadAssets() {
    FStreamableManager& streamableManager = UAssetManager::GetStreamableManager();
    int count = 0;
    int totalCount = GetAssetPaths().Num();
   for (const auto& path : GetAssetPaths()) {
       streamableManager.RequestAsyncLoad(
       path, [this, &count, totalCount]() {
           if (++count >= totalCount) { // This SHOULD work. It should hit true when the final asset is loaded.
               OnGunPartAssetsLoaded.Broadcast(this);
           }
       }
       );   
   }
}

TArray<FSoftObjectPath> UGunPartDataBase::GetAssetPaths() const {
    TArray<FSoftObjectPath> assetPaths;
    assetPaths.Add(Mesh.ToSoftObjectPath());
    TArray<TSoftObjectPtr<UMaterialInterface>> skins; Skins.GenerateValueArray(skins);
    for (const TSoftObjectPtr<UMaterialInterface>& mat : skins) {
        assetPaths.Add(mat.ToSoftObjectPath());
    }
    return assetPaths;
}

UGameplayEffect* UGunBarrelData::MakeEffect(UModularFirearmAttributeSet* attributes) const {
    if (!IsValid(attributes)) {
        return nullptr;
    }
    // Create the effect and make it instant. We want this to modify base values.
    // Do not use GetDefaultObject. Modifications here will screw up blueprints made from the class.
    UGameplayEffect* effect = NewObject<UGameplayEffect>(attributes, UGameplayEffect::StaticClass(), "Effect");
    if(IsValid(effect)){
        effect->DurationPolicy = EGameplayEffectDurationType::Instant;

        // Add Spread Multiplier
        FGameplayModifierInfo SpreadMultiplier_Modifier;
        SpreadMultiplier_Modifier.Attribute = attributes->GetSpreadMultiplierAttribute();
        SpreadMultiplier_Modifier.ModifierOp = EGameplayModOp::Override;
        SpreadMultiplier_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(DefaultSpreadMultiplier));
        effect->Modifiers.Add(SpreadMultiplier_Modifier);

        // Add Noise Modifier
        FGameplayModifierInfo Noise_Modifier;
        Noise_Modifier.Attribute = attributes->GetNoiseAttribute();
        Noise_Modifier.ModifierOp = EGameplayModOp::Override;
        Noise_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(DefaultNoise));
        effect->Modifiers.Add(Noise_Modifier);

        return effect;
    }
    return nullptr;
}

TArray<FSoftObjectPath> UGunBarrelData::GetAssetPaths() const {
     TArray<FSoftObjectPath> paths = Super::GetAssetPaths();
    paths.Add(MuzzleFlash.ToSoftObjectPath());
    paths.Add(GetFiringSound().ToSoftObjectPath());
    return paths;    
}

UGameplayEffect* UGunGripData::MakeEffect(UModularFirearmAttributeSet* attributes) const {
    if (!IsValid(attributes)) {
        return nullptr;
    }
    // Create the effect and make it instant. We want this to modify base values.
    // Do not use GetDefaultObject. Modifications here will screw up blueprints made from the class.
    UGameplayEffect* effect = NewObject<UGameplayEffect>(attributes, UGameplayEffect::StaticClass(), "Effect");
    if(IsValid(effect)){
        effect->DurationPolicy = EGameplayEffectDurationType::Instant;

        // Add Haptic Intensity
        FGameplayModifierInfo HapticIntensity_Modifier;
        HapticIntensity_Modifier.Attribute = attributes->GetHapticIntensityAttribute();
        HapticIntensity_Modifier.ModifierOp = EGameplayModOp::Override;
        HapticIntensity_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(HapticIntensity));
        effect->Modifiers.Add(HapticIntensity_Modifier);

        return effect;
    }
    return nullptr;
}

TArray<FSoftObjectPath> UGunGripData::GetAssetPaths() const {
    TArray<FSoftObjectPath> paths = Super::GetAssetPaths();
    paths.Add(HapticFeedback.ToSoftObjectPath());
    return paths;
}

UGameplayEffect* UGunMagazineData::MakeEffect(UModularFirearmAttributeSet* attributes) const {
    if (!IsValid(attributes)) {
        return nullptr;
    }
    // Create the effect and make it instant. We want this to modify base values.
    // Do not use GetDefaultObject. Modifications here will screw up blueprints made from the class.
    UGameplayEffect* effect = NewObject<UGameplayEffect>(attributes, UGameplayEffect::StaticClass(), "Effect");
    if (IsValid(effect)) {
        effect->DurationPolicy = EGameplayEffectDurationType::Instant;

        // Add Max Ammo
        FGameplayModifierInfo MaxAmmo_Modifier;
        MaxAmmo_Modifier.Attribute = attributes->GetMaxAmmoAttribute();
        MaxAmmo_Modifier.ModifierOp = EGameplayModOp::Override;
        MaxAmmo_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(MaxAmmo));
        effect->Modifiers.Add(MaxAmmo_Modifier);

        // Add Reload Speed
        FGameplayModifierInfo ReloadSpeed_Modifier;
        ReloadSpeed_Modifier.Attribute = attributes->GetReloadSpeedAttribute();
        ReloadSpeed_Modifier.ModifierOp = EGameplayModOp::Override;
        ReloadSpeed_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(ReloadSpeed));
        effect->Modifiers.Add(ReloadSpeed_Modifier);
        
        return effect;
    }    
    return nullptr;
}

TArray<FSoftObjectPath> UGunMagazineData::GetAssetPaths() const {
    TArray<FSoftObjectPath> paths = Super::GetAssetPaths();
    for (const TSoftClassPtr<AActor>& bullet : BulletClasses) {
        paths.Add(bullet.ToSoftObjectPath());
    }
    paths.Add(GetReloadMontage().ToSoftObjectPath());
    return paths;
}

UGameplayEffect* UGunSightData::MakeEffect(UModularFirearmAttributeSet* attributes) const {
    if (!IsValid(attributes)) {
        return nullptr;
    }
    // Create the effect and make it instant. We want this to modify base values.
    // Do not use GetDefaultObject. Modifications here will screw up blueprints made from the class.
    UGameplayEffect* effect = NewObject<UGameplayEffect>(attributes, UGameplayEffect::StaticClass(), "Effect");
    if (IsValid(effect)) {
        effect->DurationPolicy = EGameplayEffectDurationType::Instant;

        // Add FOV Zoom
        FGameplayModifierInfo FOVZoom_Modifier;
        FOVZoom_Modifier.Attribute = attributes->GetFOVZoomAttribute();
        FOVZoom_Modifier.ModifierOp = EGameplayModOp::Override;
        FOVZoom_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(FOVZoom));
        effect->Modifiers.Add(FOVZoom_Modifier);
        return effect;
    }
    return nullptr;
}

UGameplayEffect* UGunStockData::MakeEffect(UModularFirearmAttributeSet* attributes) const {
    if (!IsValid(attributes)) {
        return nullptr;
    }
    // Create the effect and make it instant. We want this to modify base values.
    // Do not use GetDefaultObject. Modifications here will screw up blueprints made from the class.
    UGameplayEffect* effect = NewObject<UGameplayEffect>(attributes, UGameplayEffect::StaticClass(), "Effect");
    if(IsValid(effect)) {
        effect->DurationPolicy = EGameplayEffectDurationType::Instant;

        // Add Cam Shake Intensity
        FGameplayModifierInfo CamShakeIntensity_Modifier;
        CamShakeIntensity_Modifier.Attribute = attributes->GetCamShakeIntensityAttribute();
        CamShakeIntensity_Modifier.ModifierOp = EGameplayModOp::Override;
        CamShakeIntensity_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(CamShakeIntensity));
        effect->Modifiers.Add(CamShakeIntensity_Modifier);

        // Add Swap Speed
        FGameplayModifierInfo SwapSpeed_Modifier;
        SwapSpeed_Modifier.Attribute = attributes->GetSwapSpeedAttribute();
        SwapSpeed_Modifier.ModifierOp = EGameplayModOp::Override;
        SwapSpeed_Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(SwapSpeed));
        effect->Modifiers.Add(SwapSpeed_Modifier);

        return effect;
    }
    return nullptr;
}

TArray<FSoftObjectPath> UGunStockData::GetAssetPaths() const {
    TArray<FSoftObjectPath> paths = Super::GetAssetPaths();
    paths.Add(CamShake.ToSoftObjectPath());
    return paths;    
}
