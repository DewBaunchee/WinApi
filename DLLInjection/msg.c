#include <windows.h>

#define DLL_EXPORT

#include "msg.h"

#ifdef __cplusplus
extern "C"
{
#endif

DECLDIR void Share() {
    char pid[50];
    itoa(GetCurrentProcessId(), pid, 10);
    MessageBox(NULL, pid, "Im in process number...", MB_OK);
}

#ifdef __cplusplus
}
#endif

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved) {
    BOOLEAN bSuccess = TRUE;

    switch (nReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hDllHandle);
            Share();
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return bSuccess;
}