// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_Ini.generated.h"

/**
 * 
 */
UCLASS()
class UE5MMGAME_API UBFL_Ini : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "INI")
	static FString GetGPUBrandName();

	UFUNCTION(BlueprintCallable, Category = "INI")
	static FString GetCPUBrandName();



	UFUNCTION(BlueprintCallable, Category = "INI")
	static TArray<FString> GetMountedPakFiles();

	UFUNCTION(BlueprintCallable, Category = "INI")
	static FString GetPakMountPoint(FString PakFilePath);

	UFUNCTION(BlueprintCallable, Category = "INI")
	static TArray<FString> GetRegisteredPaths();

	UFUNCTION(BlueprintCallable, Category = "INI")
	static TArray<FString> GetRegisteredClasses();

	UFUNCTION(BlueprintCallable, Category = "INI")
	static TArray<FString> GetRegisteredAssets();

	UFUNCTION(BlueprintCallable, Category = "INI")
	static TArray<FString> GetRegisteredAssetsOfClass(FString Filter);



	UFUNCTION(BlueprintCallable, Category = "INI")
	static TSoftObjectPtr<UWorld> LoadLevelClassReference(FString PakContentPath);
};
