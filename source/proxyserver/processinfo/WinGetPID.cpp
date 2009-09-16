#include <windows.h>

#include <Tlhelp32.h>
#include <Psapi.h>

#include "WinGetPID.h"

bool winParentProcessIsGUI()
{
#ifdef WINCE
	bool bIsGUI = true;
#else
	bool bIsGUI = false;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	DWORD currentProcessId;
	DWORD parentProcessId = 0;
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(hSnapshot,&processEntry))
	{
		currentProcessId = GetCurrentProcessId();

		if(processEntry.th32ProcessID == currentProcessId)
		{
			parentProcessId = processEntry.th32ParentProcessID;
		}
		else
		{
			while(Process32Next(hSnapshot,&processEntry))
			{
				if(processEntry.th32ProcessID == currentProcessId)
				{
					parentProcessId = processEntry.th32ParentProcessID;
					break;
				}
			}
		}
	}

	CloseHandle(hSnapshot);

	if(parentProcessId)
	{
		HANDLE hProcess;
		
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, parentProcessId);

		if(hProcess)
		{
			TCHAR imageFileName[MAX_PATH];
			if(GetModuleFileNameEx(hProcess,NULL,&imageFileName[0],sizeof(imageFileName)))
			{
				HANDLE hImage;

				hImage = CreateFile(imageFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

				if(hImage)
				{
					DWORD bytesRead;
					DWORD CoffHeaderOffset;
					DWORD MoreDosHeader[16];

					ULONG ntSignature;

					IMAGE_DOS_HEADER image_dos_header;
					IMAGE_FILE_HEADER image_file_header;
					IMAGE_OPTIONAL_HEADER image_optional_header;

					//Read the MS-DOS image header.
					ReadFile(hImage,&image_dos_header,sizeof(IMAGE_DOS_HEADER),&bytesRead,NULL);

					if(IMAGE_DOS_SIGNATURE==image_dos_header.e_magic)
					{
						//Read more MS-DOS header.
						ReadFile(hImage,MoreDosHeader,sizeof(MoreDosHeader),&bytesRead,NULL);

						//Get actual COFF header.
						CoffHeaderOffset = SetFilePointer(hImage,image_dos_header.e_lfanew,NULL,FILE_BEGIN) +
										   sizeof(ULONG);

						ReadFile(hImage,&ntSignature,sizeof(ULONG),&bytesRead,NULL);

						if(IMAGE_NT_SIGNATURE == ntSignature)
						{
							ReadFile(hImage,&image_file_header,IMAGE_SIZEOF_FILE_HEADER,&bytesRead,NULL);

							//Read optional header.
							ReadFile(hImage,&image_optional_header,sizeof(IMAGE_OPTIONAL_HEADER),&bytesRead,NULL);

							if(image_optional_header.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
							{
								bIsGUI = true;
							}
						}
					}

					CloseHandle(hImage);
				}
			}

			CloseHandle(hProcess);
		}
	}
#endif
	return(bIsGUI);
}
