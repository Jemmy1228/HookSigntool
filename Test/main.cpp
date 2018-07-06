#include <windows.h>
#include <string.h>
#include <stdio.h>

int main() {
	MessageBoxW(NULL, L"First MessageBox", L"Test", NULL);

	GetProcAddress(LoadLibraryW(L"HookSigntool.dll"), "attach");

	SYSTEMTIME time;
	GetLocalTime(&time);
	printf("%4d-%2d-%2d", time.wYear, time.wMonth, time.wDay);
}