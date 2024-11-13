
#include <iostream>
#include <Windows.h>
int main()
{
	std::cout << "Hello World!\n";
	std::cout << "Click on the window you want to hide!" << std::endl;
	HWND hwnd;
	while (true) {
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { // Check if left mouse button is pressed
			POINT point;
			GetCursorPos(&point);
			hwnd = WindowFromPoint(point);

			if (hwnd) {
				char title[128];
				GetWindowTextA(hwnd, title, 128);
				std::cout << "Window handle (HWND): " << hwnd << std::endl;
				std::cout << "Window title: " << title << std::endl;
				break;
			}
		}
		Sleep(10); // Poll every 100 milliseconds
	}

	


	char dllpath[MAX_PATH];
	GetFullPathNameA("./dll.dll", MAX_PATH, dllpath, nullptr);
	
	std::cout << "Found window! Now injecting dll.\n";
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
		return false;
	}
	LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, MAX_PATH + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!pDllPath) {
		std::cerr << "Failed to allocate memory in target process. Error: " << GetLastError() << std::endl;
		CloseHandle(hProcess);
		return false;
	}
	if (!WriteProcessMemory(hProcess, pDllPath, dllpath, MAX_PATH + 1, nullptr)) {
		std::cerr << "Failed to write memory. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return false;
	}
	FARPROC pLoadLibraryA = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (!pLoadLibraryA) {
		std::cerr << "Failed to get address of LoadLibraryA. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return false;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, pDllPath, 0, NULL);
	if (!hThread) {
		std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return false;
	}
	WaitForSingleObject(hThread, INFINITE);

	// Clean up
	VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	std::cin.get();

	return 0;
}






