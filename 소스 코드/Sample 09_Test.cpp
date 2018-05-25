#include <windows.h>

BOOL PrintProcessName()
{
	TCHAR	lpFilename[MAX_PATH] = {0,};
	DWORD	deResult = 0;

	deResult = GetModuleFileName(NULL, lpFilename, MAX_PATH);
	if(!deResult)
		return 0;

	MessageBox(NULL, lpFilename, "Hi~", MB_OK);

	return 1;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		PrintProcessName();
		break;	
	}
	return TRUE;
}