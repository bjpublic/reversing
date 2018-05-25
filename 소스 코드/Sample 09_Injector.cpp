#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

/**************************************************************************************
*
*	- FindProce( ) Function
*
*
**************************************************************************************/
DWORD FindProcess(LPCTSTR lpProcessName) {
	PROCESSENTRY32	pe;
	HANDLE			hSnapShot = INVALID_HANDLE_VALUE;
	DWORD			dwPID = 0;

	pe.dwSize = sizeof (PROCESSENTRY32);

	hSnapShot = CreateToolhelp32Snapshot (TH32CS_SNAPALL, NULL);
	if(hSnapShot == INVALID_HANDLE_VALUE)
		return dwPID;
	
	Process32First (hSnapShot, &pe);
    do																
    {
		//Process Name Comparison.
        if(!strcmp (lpProcessName, (LPCTSTR)pe.szExeFile))
        {
            dwPID = pe.th32ProcessID;
            break;
        }
    }
    while(Process32Next (hSnapShot, &pe));
	CloseHandle (hSnapShot);

	return dwPID;
}

/**************************************************************************************
*
*	- main( ) Function
*
*
**************************************************************************************/
int main( ) 
{
	// TODO: Add your control notification handler code here
	HANDLE					hProcess, hThread;
	HMODULE					hModule = NULL;
	LPTHREAD_START_ROUTINE	pTreadProc;
	DWORD					dwPID = 0;
	TCHAR					lpProName[12] = "notepad.exe";
	TCHAR					lpDllPath[12] = "C:\\Test.dll";
	LPVOID					pRemoteBuf = NULL;
	bool					bResult = FALSE;

	//Find a Process to get ProcessID.
	dwPID = FindProcess(lpProName);
	if(!dwPID)
		return 0;

	//Get Target Process Handle. 
	hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, dwPID);
	if(!hProcess)
		return 0;
	
	//Get Module Handle of the "Kernel32.dll".
	hModule = GetModuleHandle ("kernel32.dll");
	if(!hModule)
		goto end;

	//Get the address of the "LoadLibraryA( )" API.
	pTreadProc = (LPTHREAD_START_ROUTINE) GetProcAddress (hModule, "LoadLibraryA");
	if(!pTreadProc)
		goto end;
	
	//Commits the state of a region of memory.
	pRemoteBuf = VirtualAllocEx(hProcess,
								NULL, 
								strlen(lpDllPath)+1, 
								MEM_COMMIT, 
								PAGE_READWRITE);
	if(!pRemoteBuf)
		goto end;

	//Writes data to an area of memory in a specified process.
	bResult = WriteProcessMemory(	hProcess, 
									pRemoteBuf, 
									lpDllPath, 
									strlen(lpDllPath)+1, 
									NULL);
	if(!bResult)
		goto end;

	//"TestDll.dll" Module Injection.
	hThread = CreateRemoteThread(	hProcess, 
									0, 0, 
									pTreadProc, 
									pRemoteBuf, 
									0, NULL);
	if(!hThread)
		goto end;
	
	//Wait until it is completed.
	WaitForSingleObject (hThread, INFINITE);

	CloseHandle(hThread);
end:
	CloseHandle(hProcess);

	return 0;
}