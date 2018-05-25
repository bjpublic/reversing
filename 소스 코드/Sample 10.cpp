#include <windows.h>
#include <stdio.h>

/**************************************************************************************
*
**************************************************************************************/
typedef struct _THREAD_PARAM 
{
    FARPROC pFunc[3];               // Function Name.
    char    szBuf[4][128];          // Parameter.
} THREAD_PARAM, *PTHREAD_PARAM;

typedef HMODULE (WINAPI *PFLOADLIBRARYA)
(
    LPCSTR lpLibFileName
);

typedef FARPROC (WINAPI *PFGETPROCADDRESS)
(
    HMODULE hModule,
    LPCSTR lpProcName
);

typedef int (WINAPI *PFMESSAGEBOXA)
(
    HWND hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType
);

typedef BOOL (WINAPI *TERMINATEPROCESS)
(
    HANDLE hProcess,
	UINT   uExitCode
);

/**************************************************************************************
*
*	- ThreadProc( )
*
*
**************************************************************************************/
DWORD WINAPI ThreadProc(LPVOID lParam)
{
    PTHREAD_PARAM   pParam      = (PTHREAD_PARAM)lParam;
    HMODULE         hMod        = NULL;
    FARPROC         pFunc       = NULL;

    //CALL "LoadLibrary" API.
	//Load the 'user32.dll'.
    hMod = ((PFLOADLIBRARYA)pParam->pFunc[0])(pParam->szBuf[0]);
    if(!hMod)
        return 0;

    //CALL "GetProcAddress" API.
	//Obtain the address value of the "MessageBoxA" API. 
    pFunc = (FARPROC)((PFGETPROCADDRESS)pParam->pFunc[1])(hMod, pParam->szBuf[1]);
    if( !pFunc )
        return 0;

    //CALL "MessageBoxA" API.
    ((PFMESSAGEBOXA)pFunc)(NULL, pParam->szBuf[2], pParam->szBuf[3], MB_OK);
	
    //CALL "TerminateProcess" API.
    ((PFGETPROCADDRESS)pParam->pFunc[2])(NULL, 0);

    return 1;
}

/**************************************************************************************
*
*	- main( ) Function
*
*
**************************************************************************************/
int main(int argc, char *argv[]) {
    THREAD_PARAM    thParam;
	HMODULE         hMod = NULL;
    HANDLE          hProcess = NULL;
    HANDLE          hThread = NULL;
    LPVOID          pRemoteBuf[2] = {0,};
    DWORD           dwSize = 0;
	DWORD 			dwPID = 0;
	bool			bResult = FALSE;

	hMod = GetModuleHandleA("kernel32.dll");

    //set THREAD_PARAM
    thParam.pFunc[0] = GetProcAddress(hMod, "LoadLibraryA");
    thParam.pFunc[1] = GetProcAddress(hMod, "GetProcAddress");
	thParam.pFunc[2] = GetProcAddress(hMod, "ExitProcess");
    strcpy(thParam.szBuf[0], "user32.dll");
    strcpy(thParam.szBuf[1], "MessageBoxA");
    strcpy(thParam.szBuf[2], "The process will be terminated.");
    strcpy(thParam.szBuf[3], "Warning!!");

	printf("Target Process: ");
	scanf("%d", &dwPID);
	
    //Get the target process handle.
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    if (!hProcess)
		return 0;

    dwSize = sizeof(THREAD_PARAM);
	
	//Allocates memory in the Target Process.
	//The allocated memory space will contain the thread parameter.
	pRemoteBuf[0] = VirtualAllocEx(	hProcess,
                                    NULL, dwSize,
                                    MEM_COMMIT,
                                    PAGE_READWRITE);
    if(!pRemoteBuf[0])
		goto end;
	
	//Enter the thread parameter.
	bResult = WriteProcessMemory(	hProcess,
									pRemoteBuf[0],
									(LPVOID)&thParam,
									dwSize,
									NULL);
    if(!bResult)
		goto end;

    //Getting thread code size.
    dwSize = (DWORD)main - (DWORD)ThreadProc;
	
	//Allocates memory in the Target Process.
	//The allocated memory space will contain the thread code.
	pRemoteBuf[1] = VirtualAllocEx(	hProcess,
                                    NULL,
                                    dwSize,
                                    MEM_COMMIT,
                                    PAGE_EXECUTE_READWRITE);
    if(!pRemoteBuf[1])
		goto end;

	//Enter the thread code.
	bResult = WriteProcessMemory(	hProcess,
									pRemoteBuf[1],
									(LPVOID)ThreadProc,
									dwSize,
									NULL);
    if(!bResult)
		goto end;

	hThread = CreateRemoteThread(hProcess, NULL, 0,
                                (LPTHREAD_START_ROUTINE)pRemoteBuf[1],	//ThreadProc Address
                                pRemoteBuf[0],							//Thread Parameter Address
                                0, NULL);
    if(!hThread)
		goto end;

    WaitForSingleObject(hThread, INFINITE);	

    CloseHandle(hThread);
end:
    CloseHandle(hProcess);

	return 1;
}