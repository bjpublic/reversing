#include <windows.h>

int main( ) {
	HWND	hIEFrame = NULL,
			hWorkerW = NULL,
			hReBarWindow32 = NULL,
			hAddressBandRoot = NULL,
			hEdit = NULL;
	TCHAR	lpIEPath[MAX_PATH] = {0, },
			lpIEAddress[MAX_PATH] = "https://www.naver.com/";
	DWORD	dwResult = 0;

	dwResult = ExpandEnvironmentStrings("%ProgramFiles%", 
										lpIEPath, 
										MAX_PATH);
	if(!dwResult)
		goto End;

	strcat(lpIEPath, "\\Internet Explorer\\iexplore.exe");

	dwResult = WinExec(lpIEPath, SW_SHOW);
	if(dwResult<31)
		goto End;

	Sleep(700);

		
	//Find the IEFrame of the Internet Explorer.
	hIEFrame = FindWindow("IEFrame", NULL);
	if(!hIEFrame)
		goto End;

	//Find the WorkerW of the Internet Explorer.
	hWorkerW = FindWindowEx(hIEFrame, NULL, "WorkerW", NULL);
	if(!hWorkerW)
		goto End;

	//Find the ReBarWindow32 of the Internet Explorer.
	hReBarWindow32 = FindWindowEx(hWorkerW, NULL, "ReBarWindow32", NULL);
	if(!hReBarWindow32)
		goto End;

	//Find the Address Band Root of the Internet Explorer.
	hAddressBandRoot = FindWindowEx(hReBarWindow32, NULL, "Address Band Root", NULL);
	if(!hAddressBandRoot)
		goto End;

	//Find the Edit the Internet Explorer.
	hEdit = FindWindowEx(hAddressBandRoot, NULL, "Edit", NULL);
	if(!hEdit)
		goto End;

	//Enter the address in the address bar & Run.
	SendMessage(hEdit, WM_SETTEXT, 255, (LPARAM)lpIEAddress);
	SendMessage(hEdit, WM_KEYDOWN, VK_RETURN, NULL);
	
End:
	return 1;
}