#include <windows.h>

int main( ) 
{
	Beep(0x200, 0x300);

	MessageBoxA(NULL, "Hi, Have a nice day!", "SecurityFactory", 0);

	return 1;
}