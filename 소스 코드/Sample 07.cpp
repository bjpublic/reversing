#include <windows.h>
#include <wininet.h>
#include <stdio.h>

int main( ) {
	HINTERNET	hInternet, hUrl;
	HANDLE		hFile;
	DWORD		dwRead = 0,
				dwWritten = 0,
				dwSize = 0;
	TCHAR 		lpUrlPath[MAX_PATH] = "https://live.sysinternals.com/procexp.exe",
				lpBuffer[100000],
				lpProcPath[256];
	bool		bResult = FALSE;
	
	GetTempPath(256, lpProcPath);
	strcat(lpProcPath, "\procexp.exe");
	
	hFile = CreateFile(	lpProcPath,
						GENERIC_WRITE, 
						0, NULL, 
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	
	if(hFile != INVALID_HANDLE_VALUE)
	{
		hInternet = InternetOpen(	"HTTP", 
									INTERNET_OPEN_TYPE_PRECONFIG, 
									NULL, NULL, 0);
		if(!hInternet)
		{
			printf("CALL FAIL_InternetOpen!!\n");
			return 0;
		}
		
		hUrl = InternetOpenUrl(	hInternet,
								lpUrlPath, 
								NULL, 0, 
								INTERNET_FLAG_RELOAD, 0);
		if(!hUrl)
		{
			printf("CALL FAIL_InternetOpenUrl!!\n");
			return 0;
		}
	
		//Repeat until the download is complete. 
		do {
			//Make sure that the data is ready to be downloaded.
			bResult = InternetQueryDataAvailable(hUrl, &dwSize, 0, 0);
			if(!bResult)
			{
				printf("CALL FAIL_InternetQueryDataAvailable!!\n");
				return 0;
			}
													
			//Read Data From URL.
			bResult = InternetReadFile (hUrl, lpBuffer, dwSize, &dwRead);
			if(!bResult)
			{
				printf("CALL FAIL_InternetReadFile!!\n");
				return 0;
			}		
			
			//Write File Data.
			WriteFile (hFile, lpBuffer, dwRead, &dwWritten, NULL);	
		} while (dwRead != 0);
		
		InternetCloseHandle (hInternet);
		InternetCloseHandle (hUrl);
	}
	CloseHandle (hFile);
	
	ShellExecute(NULL, "open", lpProcPath, NULL, NULL, SW_SHOW); 

	return 1;
}