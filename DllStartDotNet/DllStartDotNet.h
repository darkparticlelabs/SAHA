#ifndef _DLLSTARTDOTNET
#define _DLLSTARTDOTNET
#endif
#include <Windows.h>
#define DllExport   __declspec( dllexport ) 
#ifdef __cplusplus 
extern "C" {
#endif
DllExport void _cdecl StartDotNet(LPCWSTR); 
#ifdef __cplusplus 
}
#endif