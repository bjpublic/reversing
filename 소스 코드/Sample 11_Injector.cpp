#include <windows.h>
#include <tlhelp32.h>


/**************************************************************************************
*
*	- InjectDll( ) Function
*
*
**************************************************************************************/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE 					hProcess, hThread;
	HMODULE 				hModule = NULL;
	LPTHREAD_START_ROUTINE 	pTreadProc;
	LPVOID 					pRemoteBuf = NULL;
	TCHAR 					lpDllPath[_MAX_PATH] = {0,};
	bool					bResult = FALSE;
	
	hModule = GetModuleHandle ("kernel32.dll");
	if(!hModule)
		return FALSE;

	pTreadProc = (LPTHREAD_START_ROUTINE) GetProcAddress (hModule, "LoadLibraryA");
	
	hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, dwPID);
	if(!hProcess)
		return FALSE;
	
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, strlen(szDllPath)+1, MEM_COMMIT, PAGE_READWRITE);
	if(!pRemoteBuf)
		goto end;

	wsprintf(lpDllPath, "%s", szDllPath);
	bResult = WriteProcessMemory(hProcess, pRemoteBuf, lpDllPath, strlen(szDllPath)+1, NULL);
	if(!bResult)
		goto end;

	hThread = CreateRemoteThread (	hProcess, 0, 0, 
									pTreadProc, 
									pRemoteBuf, 
									0, NULL);
	if(!hThread)
		goto end;
	
	WaitForSingleObject (hThread, INFINITE);

	CloseHandle(hThread);
end:
	CloseHandle(hProcess);
	CloseHandle(hModule);

	return TRUE;
}

/**************************************************************************************
*
*	- main( ) Function
*
*
**************************************************************************************/
int main()
{
	HANDLE 			hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 	pe;
	DWORD 			dwPID = 0;
	TCHAR 			lpPath[_MAX_PATH] = {0,}, 
					lpDllPath[_MAX_PATH] = {0,};

	pe.dwSize = sizeof (PROCESSENTRY32);
	
	hSnapShot = CreateToolhelp32Snapshot (TH32CS_SNAPALL, NULL);
	if(hSnapShot == INVALID_HANDLE_VALUE)
		return 0;
	
	Process32First (hSnapShot, &pe);
    do																
    {																
        if(!strcmp ("notepad.exe", (LPCTSTR)(strlwr(pe.szExeFile))))
        {
            dwPID = pe.th32ProcessID;
			
			InjectDll(dwPID, "Hook.dll");
			
            break;
        }
    }
    while(Process32Next (hSnapShot, &pe));
	
	CloseHandle (hSnapShot);

	return 1;
}