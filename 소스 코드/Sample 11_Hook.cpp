#include <stdio.h>
#include <iostream.h>
#include <windows.h>

BYTE g_pOrgCPIW[5] = {0,};

typedef BOOL (WINAPI *PFWRITEFILE)(
	HANDLE hFile,
	LPCVOID lpBuffer, 
	DWORD nNumberOfBytesToWrite, 
	LPDWORD lpNumberOfBytesWritten, 
	OVERLAPPED lpOverlapped
);

BOOL HookCode(
	LPCSTR szDllName, 
	LPCSTR szFuncName, 
	PROC pFuncNew, 
	PBYTE pOrgBytes
)
{
	FARPROC pFuncOrg;
	DWORD dwOldProtect, dwAddress;
	byte pBuf[5] = {0xE9, 0, };

	pFuncOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);

	VirtualProtect((LPVOID)pFuncOrg, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	memcpy(pOrgBytes, pFuncOrg, 5);

	dwAddress = (DWORD)pFuncNew - (DWORD)pFuncOrg - 5;

	memcpy(&pBuf[1], &dwAddress, 4);

	memcpy(pFuncOrg, pBuf, 5);

	VirtualProtect((LPVOID)pFuncOrg, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}

BOOL UnhookCode(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgBytes)
{
	FARPROC pFuncOrg;
	DWORD dwOldProtect;

	pFuncOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);

	VirtualProtect((LPVOID)pFuncOrg, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	memcpy(pFuncOrg, pOrgBytes, 5);

	VirtualProtect((LPVOID)pFuncOrg, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}

BOOL WINAPI NewWriteFile(
	HANDLE hFile,
	LPCVOID lpBuffer, 
	DWORD nNumberOfBytesToWrite, 
	LPDWORD lpNumberOfBytesWritten, 
	OVERLAPPED lpOverlapped
)
{
	BOOL bRet;
	FARPROC pFunc;
	TCHAR lpCharVal[_MAX_PATH] = {0,};
	TCHAR *p = NULL;
	int iLeng = 0;

	//unhook 호출
	UnhookCode("kernel32.dll", "WriteFile", g_pOrgCPIW);

	wsprintf(lpCharVal, "%s", lpBuffer);
	iLeng = strlen(lpCharVal);
	
	for(int i = 0; i < iLeng; i++)
		lpCharVal[i] = lpCharVal[i]^0xFF;

	pFunc = GetProcAddress(GetModuleHandleA("kernel32.dll"), "WriteFile");

	//Original Code 호출
	bRet = ((PFWRITEFILE)pFunc)(hFile,
								lpCharVal, 
								nNumberOfBytesToWrite, 
								lpNumberOfBytesWritten, 
								lpOverlapped);

	//hook 코드 호출
	HookCode("kernel32.dll", "WriteFile", (PROC)NewWriteFile, g_pOrgCPIW);

	_asm
	{
	mov esp, ebp
	}

	return bRet;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//hook 코드 호출
		HookCode("kernel32.dll", "WriteFile", (PROC)NewWriteFile, g_pOrgCPIW);

		break;
	case DLL_PROCESS_DETACH:
		//unhook 코드 호출
		UnhookCode("kernel32.dll", "WriteFile", g_pOrgCPIW);

		break;	
	}
	return TRUE;
}