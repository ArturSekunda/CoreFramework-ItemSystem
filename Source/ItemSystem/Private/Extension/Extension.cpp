// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Extension/Extension.h"

void UExtension::GetSaveData_Implementation(FSaveExtensionData& OutData)
{
	OutData.ExtensionClass = GetClass();
		
	FMemoryWriter MemoryWriter(OutData.ByteData, true);
		
	FSaveGameArchive Archive(MemoryWriter);
		
	Serialize(Archive);
}

void UExtension::LoadFromSaveData_Implementation(const FSaveExtensionData& InData)
{
	
	FMemoryReader MemoryReader(InData.ByteData, true);
	
	FSaveGameArchive Archive(MemoryReader);
	
	Serialize(Archive);
	
}
