// DllStartDotNet.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DllStartDotNet.h"
#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

ICLRRuntimeHost* GetRuntimeHost(LPCWSTR dotNetVersion);

void StartDotNet(LPCWSTR dotNetVersion=L"v4.0.30319")
{
	if(wcsstr(dotNetVersion,L"") != 0)
	{
		dotNetVersion = L"v4.0.30319";
	}
	ICLRRuntimeHost* pRun = GetRuntimeHost(dotNetVersion);
	DWORD retval = 0;
	pRun->ExecuteInDefaultAppDomain(
		L"C:\\Users\\vesh\\Documents\\Visual Studio 2012\\Projects\\DllStartDotNet\\DotNetInjectDll\\bin\\Debug\\DotNetInjectDll.dll", 
    	L"DotNetInjectDll.Class1", 
    	L"EntryPoint", 
    	L"hello .net runtime", 
    	&retval);
	pRun->Release();
}
/// <summary>
/// Returns a pointer to a running CLR host of the specified version
/// </summary>
/// <param name="dotNetVersion">The exact version number of the CLR you want to
/// run. This can be obtained by looking in the C:\Windows\Microsoft.NET\Framework
/// directory and copy the name of the last directory.</param>
/// <returns>A running CLR host or NULL. You are responsible for calling Release() on it.</returns>
ICLRRuntimeHost* GetRuntimeHost(LPCWSTR dotNetVersion)
{
	ICLRMetaHost* metaHost = NULL;
	ICLRRuntimeInfo* info = NULL;
	ICLRRuntimeHost* runtimeHost = NULL;

	// Get the CLRMetaHost that tells us about .NET on this machine
	if (S_OK == CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&metaHost))
	{
		// Get the runtime information for the particular version of .NET
		if (S_OK == metaHost->GetRuntime(dotNetVersion, IID_ICLRRuntimeInfo, (LPVOID*)&info))
		{
			// Get the actual host
			if (S_OK == info->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&runtimeHost))
			{
				// Start it. This is okay to call even if the CLR is already running
				runtimeHost->Start();
			}
		}
	}
	if (NULL != info)
		info->Release();
	if (NULL != metaHost)
		metaHost->Release();

	return runtimeHost;
}