
#include <cstdio>
#include <vector>

#include "framework.h"

void GetAllWindowsFromProcessID(DWORD dwProcessID, std::vector<HWND>& vhWnds) {
	HWND hCurWnd = nullptr;
	do {
		hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
		DWORD dwProcID = 0;
		GetWindowThreadProcessId(hCurWnd, &dwProcID);
		if (dwProcID == dwProcessID) {
			vhWnds.push_back(hCurWnd);
			printf("Found hWnd %p\n", hCurWnd);
		}
	} while (hCurWnd != nullptr);
}



DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	char processName[MAX_PATH];
	std::vector<HWND> hwnds;

	GetAllWindowsFromProcessID(GetCurrentProcessId(), hwnds);

	for (auto hwnd : hwnds)
	{
		SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
	}


	

	return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, NULL, 0, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

