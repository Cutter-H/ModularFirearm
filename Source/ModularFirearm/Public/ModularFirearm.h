// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

MODULARFIREARM_API DECLARE_LOG_CATEGORY_EXTERN(LogModularFirearm, Log, All);

class FModularFirearmModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
