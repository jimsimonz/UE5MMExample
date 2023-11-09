// Fill out your copyright notice in the Description page of Project Settings.

//bool TreatWChar_tAsBuiltInType{ get; set; };

#include "BFL_Pak.h"
#include "IPlatformFilePak.h"

#include "HAL/FileManagerGeneric.h"
#include <Misc/PackageName.h>


//#ifndef _WCHAR_T_DEFINED
//typedef unsigned short wchar_t;
//#define _WCHAR_T_DEFINED
//#endif
 
#include "Engine/GameEngine.h"
#define MyDebug(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT(x), ##__VA_ARGS__), false);}

//#include "HAL/Platform.h"
//#include "GenericPlatform/GenericPlatform.h"
//typedef wchar_t TCHAR;
//typedef char TCHAR;
//#define UTF8_TO_TCHAR(str) (TCHAR*)FUTF8ToTCHAR((const ANSICHAR*)str).Get()
//TCHAR_TO_ANSI
//ANSI_TO_TCHAR

bool UBFL_Pak::MountPakFile(const FString& PakFilePath, const FString& PakMountPoint)
{
	//MyDebug("MountPakFile pak= %s // point= %s", *PakFilePath,*PakMountPoint)

	//int32 PakOrder = rand();
	int32 PakOrder = 0;
	bool bIsMounted = false;

	//Check to see if running in editor
#if WITH_EDITOR
	return bIsMounted;
#endif

	
	FString InputPath = PakFilePath;
	FPaths::MakePlatformFilename(InputPath);
	//MyDebug("MountPakFile[MakePlatformFilename] %s",*InputPath)

	//FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(*PakFilePath));
	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")));
	if (PakFileMgr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to get PakPlatformFile for pak file (mount): %s"), *(PakFilePath));
		return false;
	}
	if (!PakMountPoint.IsEmpty())
	{
		bIsMounted = PakFileMgr->Mount(*InputPath, PakOrder, *PakMountPoint);
	}
	else
	{
		bIsMounted = PakFileMgr->Mount(*InputPath, PakOrder);
	}
	return bIsMounted;
}

bool UBFL_Pak::UnmountPakFile(const FString& PakFilePath)
{
	//MyDebug("UnmountPakFile pak= %s", *PakFilePath)

	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")));
	if (PakFileMgr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to get PakPlatformFile for pak file (Unmount): %s"), *(PakFilePath));
		return false;
	}

	//FString InputPath = PakFilePath;
	//FPaths::MakePlatformFilename(InputPath);
	//return PakFileMgr->Unmount(*InputPath);
	//return PakFileMgr->Unmount(UTF8_TO_TCHAR(*PakFilePath));
	
	return PakFileMgr->Unmount(*PakFilePath); //!!
	//return NULL;
}

void UBFL_Pak::RegisterMountPoint(const FString& RootPath, const FString& ContentPath)
{
	//if (FPackageName::MountPointExists(RootPath) == false)
		FPackageName::RegisterMountPoint(RootPath, ContentPath);

	MyDebug("RegisterMountPoint root= %s // content= %s",*RootPath, *ContentPath)
}

void UBFL_Pak::UnRegisterMountPoint(const FString& RootPath, const FString& ContentPath)
{
	FPackageName::UnRegisterMountPoint(RootPath, ContentPath);
	//MyDebug("UnRegisterMountPoint root= %s // content= %s", *RootPath, *ContentPath)
}

FString const UBFL_Pak::GetPakMountPoint(const FString& PakFilePath)
{
	//MyDebug("GetPakMountPoint pak= %s", *PakFilePath)

	FPakFile* Pak = nullptr;

	//TRefCountPtr<FPakFile> PakFile = new FPakFile(FPlatformFileManager::Get().FindPlatformFile(*PakFilePath), *PakFilePath, false);
	TRefCountPtr<FPakFile> PakFile = new FPakFile(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")), *PakFilePath, false);
	Pak = PakFile.GetReference();
	if (Pak->IsValid())
	{
		return Pak->GetMountPoint();
	}
	return FString();
}

TArray<FString> UBFL_Pak::GetPakContent(const FString& PakFilePath, bool bOnlyCooked ) //= true
{
	//MyDebug("GetPakContent pak= %s // cooked= %b", *PakFilePath,bOnlyCooked)

	FPakFile* Pak = nullptr;

	//TRefCountPtr<FPakFile> PakFile = new FPakFile(FPlatformFileManager::Get().FindPlatformFile(*PakFilePath), *PakFilePath, false);
	TRefCountPtr<FPakFile> PakFile = new FPakFile(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")), *PakFilePath, false);
	Pak = PakFile.GetReference();
	TArray<FString> PakContent;

	if (Pak->IsValid())
	{
		FString ContentPath, PakAppendPath;
		FString MountPoint = GetPakMountPoint(PakFilePath);
		MountPoint.Split("/Content/", &ContentPath, &PakAppendPath);


		TArray<FPakFile::FFilenameIterator> Records;
		for (FPakFile::FFilenameIterator It(*Pak, false); It; ++It)
		{
			Records.Add(It);
		}

		for (auto& It : Records)
		{
			if (bOnlyCooked)
			{
				if (FPaths::GetExtension(It.Filename()) == TEXT("uasset") or FPaths::GetExtension(It.Filename()) == TEXT("umap"))
				{
					PakContent.Add(FString::Printf(TEXT("%s%s"), *PakAppendPath, *It.Filename()));
				}
			}
			else
			{
				PakContent.Add(FString::Printf(TEXT("%s%s"), *PakAppendPath, *It.Filename()));
			}
		}
	}
	return PakContent;
}

FString UBFL_Pak::GetPakMountContentPath(const FString& PakFilePath)
{
	//MyDebug("GetPakMountContentPath pak= %s", *PakFilePath)

	FString ContentPath, RightStr;
	bool bIsContent;
	FString MountPoint = GetPakMountPoint(PakFilePath);
	bIsContent = MountPoint.Split("/Content/", &ContentPath, &RightStr);
	if (bIsContent)
	{
		return FString::Printf(TEXT("%s/Content/"), *ContentPath);
	}
	// Check Pak Content for /Content/ Path
	else
	{
		TArray<FString> Content = GetPakContent(PakFilePath);
		if (Content.Num() > 0)
		{
			FString FullPath = FString::Printf(TEXT("%s%s"), *MountPoint, *Content[0]);
			bIsContent = FullPath.Split("/Content/", &ContentPath, &RightStr);
			if (bIsContent)
			{
				return FString::Printf(TEXT("%s/Content/"), *ContentPath);
			}
		}
	}
	//Failed to Find Content
	return FString("");
	
}

bool UBFL_Pak::IsPakAlreadyMounted(FString PakFilePath)
{
	TArray<FString> List;

	//FString SearchPath = PakFilePath.Replace(TEXT("/"), TEXT("\\"));
	FString SearchPath = PakFilePath;
	FPaths::MakePlatformFilename(SearchPath);

	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")));
	if (PakFileMgr != nullptr)
	{
		PakFileMgr->GetMountedPakFilenames(List);
		for (auto& Item : List)
		{
			//MyDebug("IsPakAlreadyMounted = %s vs %s", *Item, *SearchPath)	
			if (Item.Equals(SearchPath) == true)
				return true;
		}
	}
	return false;
}


void UBFL_Pak::MountAndRegisterPak(FString PakFilePath, bool& bIsMountSuccessful)
{
	MyDebug("MountAndRegisterPak pak= %s", *PakFilePath)

	/*
	FString OldGameString;
	TArray<FString> RootPaths;
	FPackageName::QueryRootContentPaths(RootPaths);
	for (const auto& RootPath : RootPaths)
	{
		FString ContentPath;
		FPackageName::TryConvertLongPackageNameToFilename(RootPath, ContentPath);
		if (RootPath.Equals(TEXT("/Game/")) == true)
			OldGameString = ContentPath;
	}
	*/


	//FString SearchPath = PakFilePath;
	//FPaths::MakePlatformFilename(SearchPath);
	//PakFilePath = SearchPath;

	//if (IsPakAlreadyMounted(PakFilePath) == false)
	{
		//FString PakRootPath = "/Game/Mods/";
		FString PakRootPath = "/Game/";

		/*
		FString onePath, twoPath;
		FString tmp = PakFilePath;
		tmp.Split("/Content/Mods/", &onePath, &twoPath);

		//MyDebug("ACHTUNG %s %s %s", *tmp, *onePath, *twoPath)
		tmp = twoPath;
		tmp.Split(".pak", &onePath, &twoPath);

		PakRootPath.Append(onePath);
		PakRootPath.Append("/");
		*/


		if (!PakFilePath.IsEmpty())
		{
			if (IsPakAlreadyMounted(PakFilePath) == false)
			{ 
				if (MountPakFile(PakFilePath, FString()))
				{
					bIsMountSuccessful = true;
					const FString MountPoint = GetPakMountContentPath(PakFilePath);
					RegisterMountPoint(PakRootPath, MountPoint);

					MyDebug("MountAndRegisterPak pak successfully mounted! [%s -> %s]", *PakRootPath, *MountPoint)
				}
			}
			else
			{
				bIsMountSuccessful = true;
				const FString MountPoint = GetPakMountContentPath(PakFilePath);
				RegisterMountPoint(PakRootPath, MountPoint);

				MyDebug("MountAndRegisterPak pak successfully mounted! [%s -> %s]", *PakRootPath, *MountPoint)
			}
		}
	}
	//else
	//	MyDebug("MountAndRegisterPak pak already mounted! %s", *PakFilePath)
}

void UBFL_Pak::UnmountAndUnregisterPak(FString PakFilePath, bool& bIsMountSuccessful)
{
	//MyDebug("UnmountAndUnregisterPak pak= %s", *PakFilePath)

	FString PakRootPath = "/Game/";
	if (!PakFilePath.IsEmpty())
	{
		//if (UnmountPakFile(PakFilePath))
		{
			bIsMountSuccessful = true;
			const FString MountPoint = GetPakMountContentPath(PakFilePath);
			UnRegisterMountPoint(PakRootPath, MountPoint);
		}
	}
}


UClass* UBFL_Pak::LoadPakObjClassReference(const FString& PakFilePath, FString PakContentPath)
{
	//MyDebug("LoadPakObjClassReference pak= %s",*PakContentPath)

	//FString PakRootPath = "/Game/Mods/";
	FString PakRootPath = "/Game/";

	/*
	FString onePath, twoPath;
	FString tmp = PakFilePath;
	tmp.Split("/Content/Mods/", &onePath, &twoPath);

	//MyDebug("ACHTUNG %s %s %s", *tmp, *onePath, *twoPath)
	tmp = twoPath;
	tmp.Split(".pak", &onePath, &twoPath);

	PakRootPath.Append(onePath);
	PakRootPath.Append("/");
	*/


	const FString FileName = Conv_PakContentPathToReferenceString(PakContentPath, PakRootPath);
	return LoadPakFileClass(FileName);
}

UClass* UBFL_Pak::LoadPakFileClass(const FString& FileName)
{
	//MyDebug("LoadPakFileClass file= %s",*FileName)

	const FString Name = FileName + TEXT(".") + FPackageName::GetShortName(FileName) + TEXT("_C");
	MyDebug("LoadPakFileClass[Class Name]= %s", *Name)
	return StaticLoadClass(UObject::StaticClass(), nullptr, *Name);
}

FString UBFL_Pak::Conv_PakContentPathToReferenceString(const FString PakContentPath, const FString PakMountPath)
{
	//MyDebug("Conv_PakContentPathToReferenceString pakContent= %s // pakMount= %s", *PakContentPath,*PakMountPath)

	return FString::Printf(TEXT("%s%s.%s"),
		*PakMountPath, *FPaths::GetBaseFilename(PakContentPath, false), *FPaths::GetBaseFilename(PakContentPath, true));
}




/*

--------------------------------------

  FString path1 = FPaths::ConvertRelativePathToFull(FString("../../../TestPaks/Content/Paks/test.pak"));
	 FString path2 = FPaths::ConvertRelativePathToFull(FString("../../../TestPaks/Content/Paks/testmaterial.pak"));

	 bool check1 = false;
	 bool check2 = false;
	 if (FCoreDelegates::OnMountPak.IsBound())
	 {
		 check1 = FCoreDelegates::OnMountPak.Execute(path1, 0, nullptr); //Number should be 0-4; specifies search order
		 check2 = FCoreDelegates::OnMountPak.Execute(path2, 0, nullptr); //Number should be 0-4; specifies search order
	 }

	 UE_LOG(LogTemp, Warning, TEXT("%s"), *path1);
	 UE_LOG(LogTemp, Warning, TEXT("%s"), *path2);

	 FString NewString1 = check1 ? "true" : "false";
	 FString NewString2 = check2 ? "true" : "false";
	 UE_LOG(LogTemp, Warning, TEXT("check 1 = %s"), *NewString1);
	 UE_LOG(LogTemp, Warning, TEXT("check 2 = %s"), *NewString2);

	 FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	 IAssetRegistry& assetRegistry = AssetRegistryModule.Get();

	 TArray<FString> ContentPaths;
	 TArray<FAssetData> data;
	 //assetRegistry.AddPath(path1);
	 FString contentRelativeDir = TEXT("/Game/Paks");
	 assetRegistry.AddPath(contentRelativeDir);
	 assetRegistry.ScanPathsSynchronous({ contentRelativeDir });
	 //assetRegistry.SearchAllAssets(true);
	 assetRegistry.GetAllAssets(data, false);

	 assetRegistry.GetAllCachedPaths(ContentPaths);
	 for (FString& data : ContentPaths)
	 {
		 UE_LOG(LogTemp, Warning, TEXT("GetAllCachedPaths: %s"), *data);
	 }

	 FString NewString = FString::FromInt(data.Num());
	 UE_LOG(LogTemp, Warning, TEXT("%s"), *NewString);

	 for (int32 i = 0; i < data.Num(); i++)
	 {
		 FString s = data[i].AssetName.ToString();
		 FString ss = data[i].AssetClass.ToString();
		 UE_LOG(LogTemp, Warning, TEXT("%s | %s"), *s, *ss);
	 }

--------------------------------------

FPakPlatformFile *PakPlatformFile;
FString PlatformFileName = FPlatformFileManager::Get().GetPlatformFile().GetName();
if (PlatformFileName.Equals(FString(TEXT("PakFile"))))
{
  PakPlatformFile = static_cast<FPakPlatformFile*>(&FPlatformFileManager::Get().GetPlatformFile());
}
else
{
  PakPlatformFile = new FPakPlatformFile;
	if (!PakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT("")))
	{
		UE_LOG(LogTemp, Error, TEXT("FPakPlatformFile failed to initialize"));
		return;
	}
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
}
TArray<FString> ArrAllMountedPakFile;
PakPlatformFile->GetMountedPakFilenames(ArrAllMountedPakFile);
for (int32 i = 0; i < ArrAllMountedPakFile.Num(); i++)
{
   FString PakFileName = ArrAllMountedPakFile[i];
   FString PakFilePathFull = FPaths::ConvertRelativePathToFull(PakFileName);
   FPakFile PakFile(PakPlatformFile, *PakFilePathFull, false);
   TArray<FString> FileList;
   FString MountPoint = PakFile.GetMountPoint();
   PakFile.FindFilesAtPath(FileList, *MountPoint, true, false, true);
   for (int32 i = 0; i < FileList.Num(); i++)
   {
	   FString AssetName = FileList[i];
	   FString AssetShortName = FPackageName::GetShortName(AssetName);
	   FString FileName, FileExt;
	   AssetShortName.Split(TEXT("."), &FileName, &FileExt);
	   FString NewAssetName = TEXT("/Game/<path_to_asset>/") + FileName + TEXT(".") + FileName;

	   FSoftObjectPath StrNewAssetRef = NewAssetName;
	   FStreamableManager AssetLoader;
	   UObject* NewLoadedObject = AssetLoader.LoadSynchronous(StrNewAssetRef);
	   if (NewLoadedObject)
	   {
		   // do something, cast to compatible type.
	   }
   }
}


*/


bool UBFL_Pak::IsRuntime()
{
#if WITH_EDITOR
	return false;
#endif
	return true;
}


struct DirectoryVisitor : public IPlatformFile::FDirectoryVisitor
{

	TArray<FString> Files;

	//This function is called for every file or directory it finds.
	bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
	{
		// did we find a Directory or a file?
		if (bIsDirectory)
		{
			UE_LOG(LogTemp, Warning, TEXT("FilePaths: Directory found: %s"), FilenameOrDirectory);
			//Files.Add(FString::Printf(TEXT("%s"), FilenameOrDirectory));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FilePaths: File Found: %s"), FilenameOrDirectory);

			FString tmp = FString::Printf(TEXT("%s"), FilenameOrDirectory);
			if (tmp.Contains(".pak"))
			{
				//FString SearchPath = tmp;
				//FPaths::MakePlatformFilename(SearchPath);
				//Files.Add(SearchPath);

				Files.Add(tmp);
			}

			//Files.Add(FString::Printf(TEXT("%s"), FilenameOrDirectory));
		}
		return true;
	}
};



TArray<FString> UBFL_Pak::GetModDirectories(const FString& modsPath)
{
	//TArray<FString> PakContent;
	

	//IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	
	//FString MyConfigDirectoryTree = FPaths::ProjectConfigDir();
	//MyConfigDirectoryTree.Append(TEXT("MyConfigDir"));

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	//Now we need to create a DirectoryVisitor 
	DirectoryVisitor Visitor;
	// The ItterateDirectory takes two arguments The directory and the Visitor we created above.
	if (FileManager.IterateDirectoryRecursively(*modsPath, Visitor))
	{
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: Directory's or files found"));
		//PakContent.Add(FString::Printf(TEXT("%s"), Visitor.FilenameOrDirectory));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: Directory did not exist or visitor returned false"));
	}

	//MyConfigDirectoryTree.
	//IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	return Visitor.Files;


	/*
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	TArray<FString> Files;
	//FString FullPathFilename = modsPath;// TEXT("<path_here>\*.uasset");
	FileManager.FindFilesRecursively(Files, *modsPath, TEXT(".pak"));

	//Files.
	//for (int32 i = 0; i != Files.Num(); ++i)
	//	{
	//		JoinedStr += StrArr[Index];
	//		JoinedStr += TEXT(" ");
	//	}



	//Files.Add(TEXT("Hello"));
	return Files;*/

	//PakContent.Add(TEXT("Hello"));
	//return PakContent;

	/*
	FPakFile* Pak = nullptr;
	TRefCountPtr<FPakFile> PakFile = new FPakFile(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")), *PakFilePath, false);
	Pak = PakFile.GetReference();
	TArray<FString> PakContent;

	if (Pak->IsValid())
	{
		FString ContentPath, PakAppendPath;
		FString MountPoint = GetPakMountPoint(PakFilePath);
		MountPoint.Split("/Content/", &ContentPath, &PakAppendPath);


		TArray<FPakFile::FFilenameIterator> Records;
		for (FPakFile::FFilenameIterator It(*Pak, false); It; ++It)
		{
			Records.Add(It);
		}

		for (auto& It : Records)
		{
			if (bOnlyCooked)
			{
				if (FPaths::GetExtension(It.Filename()) == TEXT("uasset"))
				{
					PakContent.Add(FString::Printf(TEXT("%s%s"), *PakAppendPath, *It.Filename()));
				}
			}
			else
			{
				PakContent.Add(FString::Printf(TEXT("%s%s"), *PakAppendPath, *It.Filename()));
			}
		}
	}
	return PakContent;*/
}


FString UBFL_Pak::GetModFileName(const FString modPath)
{
	return FPaths::GetCleanFilename(modPath);

	//FPaths::Split(fileName, path, fileNameWithoutExtension, extension);
	//FPaths::GetBaseFilename(filePath);
}



/* Class inspector


for(TObjectIterator< UClass > ClassIt; ClassIt; ++ClassIt)
{
	UClass* Class = *ClassIt;

	// Only interested in native C++ classes
	if(!Class->IsNative())
	{
		continue;
	}

	// Ignore deprecated
	if(Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
	{
		continue;
	}

#if WITH_EDITOR
	// Ignore skeleton classes (semi-compiled versions that only exist in-editor)
	if(FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
	{
		continue;
	}
#endif

	// Check this class is a subclass of Base
	if(!Class->IsChildOf(Base))
	{
		continue;
	}

	// Add this class
	Subclasses.Add(Class);
}


// Load the asset registry module
FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
// The asset registry is populated asynchronously at startup, so there's no guarantee it has finished.
// This simple approach just runs a synchronous scan on the entire content directory.
// Better solutions would be to specify only the path to where the relevant blueprints are,
// or to register a callback with the asset registry to be notified of when it's finished populating.
TArray< FString > ContentPaths;
ContentPaths.Add(TEXT("/Game"));
AssetRegistry.ScanPathsSynchronous(ContentPaths);

FName BaseClassName = Base->GetFName();

// Use the asset registry to get the set of all class names deriving from Base
TSet< FName > DerivedNames;
{
	TArray< FName > BaseNames;
	BaseNames.Add(BaseClassName);

	TSet< FName > Excluded;
	AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);
}


FARFilter Filter;
Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
Filter.bRecursiveClasses = true;
if(!Path.IsEmpty())
{
	Filter.PackagePaths.Add(*Path);
}
Filter.bRecursivePaths = true;

TArray< FAssetData > AssetList;
AssetRegistry.GetAssets(Filter, AssetList);


 Iterate over retrieved blueprint assets
for(auto const& Asset : AssetList)
{
	// Get the the class this blueprint generates (this is stored as a full path)
	if(auto GeneratedClassPathPtr = Asset.TagsAndValues.Find(TEXT("GeneratedClass")))
	{
		// Convert path to just the name part
		const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(*GeneratedClassPathPtr);
		const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);

		// Check if this class is in the derived set
		if(!DerivedNames.Contains(*ClassName))
		{
			continue;
		}

		// Store using the path to the generated class
		Subclasses.Add(TAssetSubclassOf< UObject >(FStringAssetReference(ClassObjectPath)));
	}
}


*/