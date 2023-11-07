// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_Ini.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#include "IPlatformFilePak.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/TopLevelAssetPath.h"

#include "HAL/FileManagerGeneric.h"
#include <Misc/PackageName.h>
#include <Kismet/GameplayStatics.h>

#include "UObject/UObjectGlobals.h"
#include "Engine/World.h"


#define MyDebug(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT(x), ##__VA_ARGS__), false);}


FString UBFL_Ini::GetGPUBrandName()
{
	return FWindowsPlatformMisc::GetPrimaryGPUBrand();
}

FString UBFL_Ini::GetCPUBrandName()
{
	return FWindowsPlatformMisc::GetCPUBrand();
}


// TMP here TMP
TArray<FString> UBFL_Ini::GetMountedPakFiles()
{
	TArray<FString> List;

	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")));
	if (PakFileMgr != nullptr)
		PakFileMgr->GetMountedPakFilenames(List);
	else
		List.Add("empty");

	return List;
}

FString UBFL_Ini::GetPakMountPoint(FString PakFilePath)
{
	FName Name= FPackageName::GetPackageMountPoint(PakFilePath);
	return Name.GetPlainNameString();
}



TArray<FString> UBFL_Ini::GetRegisteredPaths()
{
	TArray<FString> List;

	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")));
	if (PakFileMgr != nullptr)
	{
		//PakFileMgr.
	}

	/*
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.GetAllCachedPaths(List);
	*/

	return List;
}




TArray<FString> UBFL_Ini::GetRegisteredClasses()
{
	TArray<FString> List;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.GetAllAssets(AssetData);

	for (auto& Item : AssetData)
	{
		// len("/Script/Engine.") = 15

		FString Filter = Item.AssetClassPath.ToString();

		int i;
		Filter.FindLastChar(TEXT('.'), i);
		Filter = Filter.RightChop(i+1);

		if (List.Contains(Filter) == false)
		{
			List.Add(Filter);
		}
	}

	List.Sort();
	return List;
}




/*
FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

// Need to do this if running in the editor with -game to make sure that the assets in the following path are available
TArray<FString> PathsToScan;
PathsToScan.Add(TEXT("/Game/Assets/Meshes/"));
AssetRegistry.ScanPathsSynchronous(PathsToScan);

TArray<FAssetData> MeshAssetList;
AssetRegistry.GetAssetsByPath(FName("/Game/Assets/Meshes/"), MeshAssetList);
*/


//TArray<FAssetData> caller()
//{

//}


TArray<FString> UBFL_Ini::GetRegisteredAssets()
{
	return GetRegisteredAssetsOfClass(FString());

/*
	TArray<FString> List;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.GetAllAssets(AssetData);
	//AssetRegistry.GetAssetsByClass(FName("MaterialFunction"), AssetData);
			

	
	//TArray<FString> List;

	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	//TArray<FAssetData> AssetData;
	//const UClass* Class = UStaticMesh::StaticClass();
	//const UClass* Class = UStaticMaterial::StaticClass();
	//AssetRegistryModule.Get().GetAssetsByClass(Class->GetFName(), AssetData);

	FString Header = FString::Printf(TEXT("Name: %-45s   Class: %-40s        PackagePath: %-50s       ObjectPath: %s"), "", "", "", "");
	List.Add(Header);

	for (auto& Item : AssetData)
	{
		FString PakPath(Item.PackagePath.ToString());
		FString ObjPath(Item.GetObjectPathString());
		ObjPath = ObjPath.RightChop(PakPath.Len());

		FString Data = FString::Printf(TEXT("%-45s         %-40s          %-50s                    %s"),
			*Item.AssetName.ToString(),
			*Item.AssetClassPath.ToString(),
			*Item.PackagePath.ToString(), //*Item.PackageName.ToString());
			//*ObjPath);			
			*Item.GetObjectPathString()); //*Item.ObjectPath.ToString());

		
		//Out.Append("Name: ");
		//Out.Append(Item.AssetName.ToString());
		//Out.Append("\tClass:        ");
		//Out.Append(Item.AssetClass.ToString());
		//Out.Append("ObjectPath:   ");
		//Out.Append(Item.ObjectPath.ToString());
		//Out.Append("PackagePath:  ");
		//Out.Append(Item.PackagePath.ToString());

		List.Add(Data);

		//List.Add(Item.AssetName.ToString());
		//List.Add(Item.AssetClass.ToString());
		//List.Add(Item.ObjectPath.ToString());
		//List.Add(Item.PackagePath.ToString());
		//List.Add(Item.GroupNames.ToString());
	}
	
	return List;*/
}


TArray<FString> UBFL_Ini::GetRegisteredAssetsOfClass(FString Filter)
{
	TArray<FString> List;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	if (Filter.Len() == 0)
		AssetRegistry.GetAllAssets(AssetData);
	else
		AssetRegistry.GetAssetsByClass(FName(Filter), AssetData);

	FString Header = FString::Printf(TEXT("Name: %-45s   Class: %-40s   PackagePath: %-50s       ObjectPath: %s"), "", "", "", "");
	List.Add(Header);
	
	for (auto& Item : AssetData)
	{
		FString PakPath(Item.PackagePath.ToString());
		FString ObjPath(Item.GetObjectPathString());
		ObjPath = ObjPath.RightChop(PakPath.Len());

		FString Data = FString::Printf(TEXT("%-45s         %-40s          %-50s                    %s"),
			*Item.AssetName.ToString(),
			*Item.AssetClassPath.ToString(),
			*Item.PackagePath.ToString(), //*Item.PackageName.ToString());
			//*ObjPath);			
			*Item.GetObjectPathString()); //*Item.ObjectPath.ToString());
		List.Add(Data);
	}

	return List;
}





TSoftObjectPtr<UWorld> UBFL_Ini::LoadLevelClassReference(FString PakContentPath, UObject* world)
{
	MyDebug("BFL_LoadLevel trying to load = %s", *PakContentPath)

	// Debugging
	//UWORLD World = GetWorld();
	//UGameplayStatics::GetAllActorsWithTag();
	//TAssetPtrObject->IsValidLowLevel()
	//UGameplayStatics::OpenLevel(world->GetWorld(), *PakContentPath);
	//return NULL;


	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	//AssetRegistry.GetAssetsByPackageName(*PakContentPath, AssetData);
	//AssetRegistry.GetAllAssets(AssetData);
	AssetRegistry.GetAssetsByClass(FName("World"), AssetData);

	for (auto& Item : AssetData)
	{
		if (*Item.AssetName.ToString() == FName(*PakContentPath))
		//if (*Item.AssetName.ToString() == FName("PakTest"))
		//if (*Item.AssetName.ToString() == FName("ThirdPersonMap"))
		{
			MyDebug("BFL_LoadLevel: Level in registry found = %s", *PakContentPath)

			/*UObject* NewLevel = Item.GetAsset();
			if (NewLevel != NULL)
			{
				MyDebug("BFL_LoadLevel Loading GetAsset() success = %s", *PakContentPath)
				return NewLevel;
			}*/
			
			//UObject* NewLevel = StaticLoadObject(UWorld::StaticClass(), nullptr, *PakContentPath);
			//UObject* NewLevel = LoadObject<UWorld>(nullptr, *PakContentPath);
			//UObject* NewLevel = LoadObject<UWorld>(nullptr, TEXT("/Game/POC/PakTest"));
			UObject* NewLevel = LoadObject<UWorld>(nullptr, *Item.GetObjectPathString());
			if (NewLevel != NULL)
			{
				MyDebug("BFL_LoadLevel Loading StaticLoadObject() success = %s", *PakContentPath)
				return NewLevel;
			}
		}
	}




	//UWorld* TheWorld = ();
	//FString CurrentLevel = TheWorld->GetMapName();

	/*
	UGameplayStatics::OpenLevel((), TEXT(“World’ / Game / Maps / Level1.Level1’”), TRAVEL_Absolute);


	UWorld* TheWorld = ();

	FString CurrentLevel = TheWorld->GetMapName();

	if (CurrentLevel == "ThirdPersonExampleMap")
	{
		UGameplayStatics::OpenLevel((), "Level2");*/

	//UGameplayStatics::OpenLevel( ((), "Level2");
	//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

	//UGameplayStatics::OpenLevel((),TEXT(PakContentPath), TRAVEL_Absolute);


	
	//return StaticLoadClass(UWorld::StaticClass(), nullptr, TEXT("/Game/mod3/L_mod3Level_C"));
	//return StaticLoadObject(UWorld::StaticClass(), nullptr, TEXT("PakTest"));

	MyDebug("BFL_LoadLevel error")
	return NULL;
}


/* tmp tmp
template <typename ObjClass>
static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return nullptr;

	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), nullptr, *Path.ToString()));
}

static FORCEINLINE UMaterial* LoadMaterialFromPath(const FName& Path)
{
	if (Path == NAME_None) return nullptr;

	return LoadObjFromPath<UMaterial>(Path);
}
*/