// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_Ini.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#include "IPlatformFilePak.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/TopLevelAssetPath.h"
#include "Interfaces/IPluginManager.h"

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

	//TArray<FString> Output;
	TArray<FString> RootPaths;
	FPackageName::QueryRootContentPaths(RootPaths);

	List.Add(FString::Printf(TEXT("RootPath: %-40s  MountPoint: %s"), "", ""));
	for (const auto& RootPath : RootPaths)
	{
		FString ContentPath;
		FPackageName::TryConvertLongPackageNameToFilename(RootPath, ContentPath);
		//ContentPath = ContentPath.ConvertTabsToSpaces(1);
		//ContentPath.RemoveSpacesInline();
		//ContentPath= ContentPath.Replace(TEXT("../"), TEXT("x"));
		//Output.Emplace(FString::Printf(TEXT("RootPath: %s    -->    %s"), *RootPath, *ContentPath));
		List.Add(FString::Printf(TEXT("%-40s            %-500s"), *RootPath, *ContentPath));
	}

	//FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("paths.log")));
	//List.Sort();
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



TArray<FString> UBFL_Ini::GetRegisteredPlugins()
{
	TArray<FString> List;
	List.Add(FString::Printf(TEXT("plugin %-40s enabled %-10s hasContent %-10s mount %-40s   explicitlyLoaded %-10s"), "", "", "", "",""));

	auto& PluginManager = IPluginManager::Get();
	auto AllPlugins = PluginManager.GetDiscoveredPlugins();
	for (const auto& Plugin : AllPlugins)
	{
		List.Add(FString::Printf(TEXT("%-40s        %-10i         %-10i            %-40s         %-10i"), 
			*Plugin->GetName(), 
			Plugin->IsEnabled(), 
			Plugin->CanContainContent(), 
			*Plugin->GetMountedAssetPath(), 
			Plugin->GetDescriptor().bExplicitlyLoaded));
	}
	//FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("plugins.log")));

	return List;
}





TSoftObjectPtr<UWorld> UBFL_Ini::LoadLevelClassReference(FString PakContentPath, UObject* world)
{
	//MyDebug("BFL_LoadLevel trying to load = %s", *PakContentPath)

	/*
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	AssetRegistry.GetAssetsByClass(FTopLevelAssetPath("/Script/Engine.World"), AssetData);
	for (auto& Item : AssetData)
	{
		if (*Item.AssetName.ToString() == FName(*PakContentPath))
		{
			return TSoftObjectPtr<UWorld>(Item.GetObjectPathString());
		}
	}
	*/



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
			//MyDebug("BFL_LoadLevel: Level in registry found = %s", *PakContentPath)

			UObject* NewLevel = Item.GetAsset();
			if (NewLevel != NULL)
			{
				//MyDebug("BFL_LoadLevel Loading GetAsset() success = %s", *PakContentPath)
				return NewLevel;
			}
			
			//UObject* NewLevel = StaticLoadObject(UWorld::StaticClass(), nullptr, *PakContentPath);
			//UObject* NewLevel = LoadObject<UWorld>(nullptr, *PakContentPath);
			//UObject* NewLevel = LoadObject<UWorld>(nullptr, TEXT("/Game/POC/PakTest"));
			NewLevel = LoadObject<UWorld>(nullptr, *Item.GetObjectPathString());
			if (NewLevel != NULL)
			{
				//MyDebug("BFL_LoadLevel Loading StaticLoadObject() success = %s", *PakContentPath)
				return NewLevel;
			}
		}
	}
	
	// Spawn Level Class
	FString LevelClass;
	LevelClass.Append(PakContentPath);
	LevelClass.Append(".");
	LevelClass.Append(PakContentPath);
	LevelClass.Append(".");
	LevelClass.Append(PakContentPath);
	LevelClass.Append(".");
	LevelClass.Append(PakContentPath);
	LevelClass.Append("_C");

	
	UObject* NewLevel = StaticLoadClass(UObject::StaticClass(), nullptr, *LevelClass);
	if (NewLevel != NULL)
	{
		//MyDebug("BFL_LoadLevel creating LevelClass success = %s", *LevelClass)
			return NewLevel;
	}
	else
	{
		//MyDebug("BFL_LoadLevel creating LevelClass failed = %s", *LevelClass)
	}



	// Return virtual object path
	LevelClass = FString("/Game/Mod2/");
	LevelClass.Append(PakContentPath);
	//LevelClass.Append(".");
	//LevelClass.Append(PakContentPath);
	//LevelClass.Append("_C");

	//MyDebug("BFL_LoadLevel just by objectPath = %s", *LevelClass)

	//NewLevel = LoadObject<UWorld>(nullptr, *LevelClass);
	return TSoftObjectPtr<UWorld>(LevelClass);





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


	//UGameplayStatics::OpenLevel(GetWorld(), FName("PakTest"));
	MyDebug("BFL_LoadLevel error")
	return NULL;
}


UObject* UBFL_Ini::LoadAssetClassReference(FString PakContentPath)
{
	MyDebug("BFL_LoadAsset trying to load = %s", *PakContentPath)

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	//AssetRegistry.GetAssetsByPackageName(*PakContentPath, AssetData);
	AssetRegistry.GetAllAssets(AssetData);
	//AssetRegistry.GetAssetsByClass(FName("World"), AssetData);

	for (auto& Item : AssetData)
	{
		if (*Item.AssetName.ToString() == FName(*PakContentPath))
		{
			MyDebug("BFL_LoadAsset: Asset in registry found = %s", *PakContentPath)

			UObject* NewAsset = Item.GetAsset();
			if (NewAsset != NULL)
			{
				MyDebug("BFL_LoadAsset Loading GetAsset() success = %s", *PakContentPath)
					return NewAsset;
			}

			NewAsset = LoadObject<UObject>(nullptr, *Item.GetObjectPathString());
			if (NewAsset != NULL)
			{
				MyDebug("BFL_LoadAsset Loading StaticLoadObject() success = %s", *PakContentPath)
					return NewAsset;
			}
		}
	}
	MyDebug("BFL_LoadAsset error")
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


/*
// 4.15 IWYU includes
#include "HAL/PlatformFilemanager.h"
#include "HAL/FileManager.h"
#include "Engine/StreamableManager.h"
#include "AssetRegistryModule.h"
#include "IPlatformFilePak.h"

void UUtils::LoadAssetsFromPakFolder(
	const FString				&DLCFolder,
	TArray<TAssetPtr<UObject> > &Assets,
	TArray<UClass*>				&Classes)
{
	UE_LOG(PaksLoadingLog, Log, TEXT("============= START LOADING PAKS ============="));
	UE_LOG(PaksLoadingLog, Log, TEXT("\n\n\tGameContentDir: %s\n\tEngineContentDir: %s"),
		*FPaths::GameContentDir(),
		*FPaths::EngineContentDir());

	FString Folder = DLCFolder;
	FPaths::NormalizeDirectoryName(Folder);

	TArray<FString> FileNames;

	IPlatformFile* PreviousPlatformFile = NULL;
	FPakPlatformFile *PlatformFile = NULL;
	if (FString(FPlatformFileManager::Get().GetPlatformFile().GetName()).Equals(FString(TEXT("PakFile"))))
	{
		PlatformFile = static_cast<FPakPlatformFile*>(&FPlatformFileManager::Get().GetPlatformFile());
	}
	else
	{
		PlatformFile = new FPakPlatformFile;

		if (!PlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT("")))
		{
			UE_LOG(PaksLoadingLog, Error, TEXT("FPakPlatformFile failed to initialize"));
			return;
		}
		PreviousPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
		FPlatformFileManager::Get().SetPlatformFile(*PlatformFile);
	}

	if (!PlatformFile->DirectoryExists(*Folder))
	{
		UE_LOG(PaksLoadingLog, Error, TEXT("Directory not found: %s"), *Folder);
		return;
	}
	UE_LOG(PaksLoadingLog, Log, TEXT("DLC directory found: %s"), *Folder);

	// Get list of files in folder
	IFileManager &FileManager = IFileManager::Get();
	FileManager.FindFiles(FileNames, *Folder);

	UE_LOG(PaksLoadingLog, Log, TEXT("Iterating files in DLC folder"));

	FStreamableManager StreamableManager;

	// Iterates on every file, mount the paks and load the assets found in them
	for (FString &FileName : FileNames)
	{
		// Check if extension match required format
		if (FileName.Right(3).Equals(TEXT("pak"), ESearchCase::IgnoreCase))
		{
			UE_LOG(PaksLoadingLog, Log, TEXT("Pak file found: %s"), *FileName);
			FString PakFullPath( FPaths::Combine(*Folder, *FileName) );
			FString PakName, PakExtension;
			FileName.Split( TEXT("."), &PakName, &PakExtension );

			FString PakMountPoint = FPaths::Combine(FPaths::GameContentDir(), TEXT("DLC"), PakName);

			if ( !PlatformFile->Mount(*PakFullPath, 0, *PakMountPoint) )
			{
				UE_LOG(PaksLoadingLog, Log, TEXT("Failed to mount %s"), *FileName);
				continue;
			}

			FString VirtualMountPoint(FString::Printf(TEXT("/Game/DLC/%s/"), *PakName));
			FPackageName::RegisterMountPoint(VirtualMountPoint, PakMountPoint);
			UE_LOG(PaksLoadingLog, Log, TEXT("Mount successful. Iterating assets in %s"), *VirtualMountPoint);

			// Print files in pak
			struct FilesDump : public IPlatformFile::FDirectoryVisitor
			{
				FString mp_PakName;
				TArray<FString> Files;

				FilesDump(FString &PakName)
					: mp_PakName(PakName)
				{}

				virtual bool Visit(const TCHAR *FilenameOrDirectory, bool bIsDirectory)
				{
					if (bIsDirectory)
					{
						UE_LOG(PaksLoadingLog, Log, TEXT("Found DIR in %s: %s"), *mp_PakName, FilenameOrDirectory);
					}
					else
					{
						const FString Filename(FilenameOrDirectory);
						if (Filename.Contains(TEXT(".umap")) || Filename.Contains(TEXT(".uasset")))
						{
							Files.Add(FilenameOrDirectory);
							UE_LOG(PaksLoadingLog, Log, TEXT("Found FILE in %s: %s"), *mp_PakName, FilenameOrDirectory);
						}
					}
					return true;
				}
			};

			FilesDump Visitor(FileName);
			PlatformFile->IterateDirectoryRecursively(*PakMountPoint, Visitor);

			for (const FString &AssetFilePath : Visitor.Files)
			{
				UE_LOG(PaksLoadingLog, Log, TEXT("Asset File: %s"), *AssetFilePath);

				FString AssetFileName = FPackageName::GetShortName(AssetFilePath);
				FString AssetName, AssetExtension;
				// Get asset name and extension
				AssetFileName.Split(TEXT("."), &AssetName, &AssetExtension);
				// Replace system path for virtual path and extension for asset name
				FString AssetRefPath(AssetFilePath.Replace(*PakMountPoint, *VirtualMountPoint));
				AssetRefPath = AssetRefPath.Replace(*AssetExtension, *AssetName);
				FPaths::RemoveDuplicateSlashes(AssetRefPath);

				// If asset is a Blueprint
				if (AssetName.StartsWith(TEXT("BP_")))
				{
					// Add class postfix
					AssetRefPath += TEXT("_C");
					UObject* LoadedClass = StreamableManager.SynchronousLoad(FStringAssetReference(AssetRefPath));
					if (LoadedClass == NULL)
					{
						UE_LOG(PaksLoadingLog, Error, TEXT("Failed to load class: %s"), *AssetRefPath);
						continue;
					}
					UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(LoadedClass);
					if (BPClass == NULL)
					{
						UE_LOG(PaksLoadingLog, Error, TEXT("Casting to BlueprintGeneratedClass failed: %s"), *AssetRefPath);
						continue;
					}
					Classes.Add(BPClass);
				}
				else
				{
					Assets.Add(TAssetPtr<UObject>(FStringAssetReference(AssetRefPath)));
				}
			}
		}
	}

	// return previous platform file manager to the top of the chain, so Unreal doesn't lose it's references
	if (PreviousPlatformFile != NULL)
	{
		FPlatformFileManager::Get().SetPlatformFile(*PreviousPlatformFile);
	}

	UE_LOG(PaksLoadingLog, Log, TEXT("============= END LOADING PAKS ============="));
}
*/


/*
#include "Interfaces/IPluginManager.h"
#include "IPlatformFilePak.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryHelpers.h"

void DebugPlugins()
{
	TArray<FString> Output;
	auto& PluginManager = IPluginManager::Get();
	auto AllPlugins = PluginManager.GetDiscoveredPlugins();
	for (const auto& Plugin : AllPlugins)
	{
		Output.Emplace(FString::Printf(TEXT("plugin:%s, enabled:%i, hasContent:%i, mount:%s, explicitlyLoaded:%i"), *Plugin->GetName(), Plugin->IsEnabled(), Plugin->CanContainContent(), *Plugin->GetMountedAssetPath(), Plugin->GetDescriptor().bExplicitlyLoaded));
	}
	FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("plugins.log")));
}

void DebugPaks()
{
	TArray<FString> Output;
	auto PakPlatform = (FPakPlatformFile*)FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile"));
	if (PakPlatform != nullptr)
	{
		TArray<FString> Paks;
		PakPlatform->GetMountedPakFilenames(Paks);
		for (const auto& Pak : Paks)
		{
			Output.Emplace(FString::Printf(TEXT("pak: %s"), *Pak));
		}
	}
	else
	{
		Output.Emplace("PakPlatformFile = nullptr");
	}
	FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("paks.log")));
}

void DebugVirtualPaths()
{
	TArray<FString> Output;
	TArray<FString> RootPaths;
	FPackageName::QueryRootContentPaths(RootPaths);
	for (const auto& RootPath : RootPaths)
	{
		FString ContentPath;
		FPackageName::TryConvertLongPackageNameToFilename(RootPath, ContentPath);
		Output.Emplace(FString::Printf(TEXT("RootPath: %s    -->    %s"), *RootPath, *ContentPath));
	}
	FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("paths.log")));
}

void DumpAssets()
{
	TArray<FString> Output;
	auto AR = IAssetRegistry::Get();
	auto DumpAssetsInPath = [&AR, &Output](const FString& Path) {
		Output.Emplace(FString::Printf(TEXT("--- %s"), *Path));
		TArray<FAssetData> Assets;
		AR->GetAssetsByPath(*Path, Assets, true);
		for (const auto& Asset : Assets)
		{
			Output.Emplace(FString::Printf(TEXT("Asset: %s"), *Asset.GetFullName()));
		}
	};
	DumpAssetsInPath("/Game");
	DumpAssetsInPath("/Mod1");
	FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("assets.log")));
}

void DebugChildBlueprints()
{
	TArray<FString> Output;
	auto AR = IAssetRegistry::Get();
	TArray<FAssetData> Assets;
	FARFilter Filter;
	Filter.TagsAndValues.Add("ParentClass", FString("/Script/Engine.BlueprintGeneratedClass'/Game/BP_Base.BP_Base_C'"));
	AR->GetAssets(Filter, Assets);
	for (const auto& Asset : Assets)
	{
		Output.Emplace(FString::Printf(TEXT("Asset: %s"), *Asset.GetFullName()));
	}
	FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("childbp.log")));
}

void DebugStaticLoad()
{
	TArray<FString> Output;
	auto StaticLoad = [&Output](const FString& Path) {
		UObject* Obj = StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
		Output.Emplace(FString::Printf(TEXT("StaticLoad('%s'): %s"), *Path, Obj ? *Obj->GetFullName() : TEXT("NULL")));
	};
	StaticLoad("/Game/BP_GameDerived.BP_GameDerived_C");
	StaticLoad("/Game/Mod1/BP_ModDerived.BP_ModDerived_C");
	StaticLoad("/Mod1/BP_ModDerived.BP_ModDerived_C");
	FFileHelper::SaveStringArrayToFile(Output, *FPaths::Combine(FPaths::ProjectDir(), TEXT("staticload.log")));
}
*/