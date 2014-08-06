// InjectExe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;
DWORD_PTR Inject(const HANDLE, const LPVOID, const wstring& );
DWORD_PTR GetRemoteModuleHandle(const int, const wchar_t*);
DWORD_PTR GetFunctionOffset(const wstring&, const char*);
int _tmain(int argc, _TCHAR* argv[])
{
	PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo ; //Only compulsory field
	if(CreateProcess(L"c:\\windows\\system32\\notepad.exe", NULL, 
		NULL,NULL,FALSE,0,NULL,
		NULL,&StartupInfo,&ProcessInfo))
	{ 
		WaitForSingleObject(ProcessInfo.hProcess,1000);
		
		// get handle to remote process
		//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.dwProcessId);

		// get address of LoadLibrary
		FARPROC fnLoadLibrary = GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryW");
		Inject(ProcessInfo.hProcess,fnLoadLibrary,L"C:\\Users\\vesh\\Documents\\Visual Studio 2012\\Projects\\DllStartDotNet\\x64\\Debug\\DllStartDotNet.dll");

		DWORD_PTR pDw = GetRemoteModuleHandle(ProcessInfo.dwProcessId,L"DllStartDotNet.dll");
		DWORD_PTR offset = GetFunctionOffset(L"C:\\Users\\vesh\\Documents\\Visual Studio 2012\\Projects\\DllStartDotNet\\x64\\Debug\\DllStartDotNet.dll", "StartDotNet");
		DWORD_PTR fnImplant = pDw + offset;
		CreateRemoteThread(ProcessInfo.hProcess,NULL,NULL,(LPTHREAD_START_ROUTINE)fnImplant,
			(LPVOID)pDw,0,0);
		//get rid of the process since the point has been made
		//TerminateProcess(ProcessInfo.hProcess,1);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}  

	return 0;
}
DWORD_PTR GetFunctionOffset(const wstring& library, const char* functionName)
{
    // load library into this process
    HMODULE hLoaded = LoadLibrary(library.c_str());
    
    // get address of function to invoke
    void* lpInject = GetProcAddress(hLoaded, functionName);
    
    // compute the distance between the base address and the function to invoke
    DWORD_PTR offset = (DWORD_PTR)lpInject - (DWORD_PTR)hLoaded;
    
    // unload library from this process
    FreeLibrary(hLoaded);
    
    // return the offset to the function
    return offset;
}
DWORD_PTR GetRemoteModuleHandle(const int processId, const wchar_t* moduleName)
{
    MODULEENTRY32 me32; 
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    
    // get snapshot of all modules in the remote process 
    me32.dwSize = sizeof(MODULEENTRY32); 
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    
    // can we start looking?
    if (!Module32First(hSnapshot, &me32)) 
    {
    	CloseHandle(hSnapshot);
    	return 0;
    }
    
    // enumerate all modules till we find the one we are looking for 
    // or until every one of them is checked
    while (wcscmp(me32.szModule, moduleName) != 0 && Module32Next(hSnapshot, &me32));
    
    // close the handle
    CloseHandle(hSnapshot);
    
    // check if module handle was found and return it
    if (wcscmp(me32.szModule, moduleName) == 0)
    	return (DWORD_PTR)me32.modBaseAddr;
    
    return 0;
}
DWORD_PTR Inject(const HANDLE hProcess, const LPVOID function, 
				 const wstring& argument)
{
	// allocate some memory in remote process
	int length = wcslen(argument.c_str());
	LPVOID baseAddress = VirtualAllocEx(hProcess, NULL, wcslen(argument.c_str())*sizeof(wchar_t), 
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// write argument into remote process	
	BOOL isSucceeded = WriteProcessMemory(hProcess, baseAddress, argument.c_str(), 
		wcslen(argument.c_str())*sizeof(wchar_t), NULL);

	// make the remote process invoke the function
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
		(LPTHREAD_START_ROUTINE)function, baseAddress, NULL, 0);

	// wait for thread to exit
	WaitForSingleObject(hThread, INFINITE);

	// free memory in remote process
	VirtualFreeEx(hProcess, baseAddress, 0, MEM_RELEASE);

	// get the thread exit code
	DWORD exitCode = 0;
	GetExitCodeThread(hThread, &exitCode);

	// close thread handle
	CloseHandle(hThread);

	// return the exit code
	return exitCode;
}