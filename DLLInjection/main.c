#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define DLL_PATH "D:\\University\\WinApi\\DLLInjection\\msg.dll"

DWORD getProcessId(const char *name) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (strcmp(entry.szExeFile, name) == 0)
                return entry.th32ProcessID;
    CloseHandle(snapshot);
    return 0;
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("Specify process name.\n");
        return -1;
    }

    printf("Injector started...\n");

    DWORD targetPid = getProcessId(argv[1]);
    if (!targetPid) {
        printf("Cannot find process.\n");
        return -1;
    }
    printf("Process founded.\n");

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPid);
    if (!hProcess) {
        printf("Cannot handle process.\n");
        return (int) GetLastError();
    }
    printf("Handle received: %d.\n", (int) hProcess);

    LPTHREAD_START_ROUTINE funcLoadLibrary =
            (LPTHREAD_START_ROUTINE) GetProcAddress(
                    GetModuleHandleA("Kernel32.dll"),
                    "LoadLibraryA"
            );
    if (!funcLoadLibrary) {
        printf("Failed to retrieve a 'LoadLibraryA' function.\n");
        return (int) GetLastError();
    }
    printf("'LoadLibraryA' founded.\n");

    LPVOID dllPath = VirtualAllocEx(hProcess, 0, strlen(DLL_PATH), MEM_COMMIT, PAGE_READWRITE);
    if (!dllPath) {
        printf("Failed to allocate memory in the target process.\n");
        return (int) GetLastError();
    }
    printf("Virtual memory allocated.\n");

    if (!WriteProcessMemory(hProcess, dllPath, (LPVOID) DLL_PATH, strlen(DLL_PATH), NULL)) {
        printf("Failed to write in process memory.\n");
        return (int) GetLastError();
    }

    HANDLE hRemoteThread = CreateRemoteThread(
            hProcess,
            NULL,
            0,
            funcLoadLibrary,
            dllPath,
            0,
            NULL
    );
    if (!hRemoteThread || hRemoteThread == INVALID_HANDLE_VALUE) {
        printf("Failed to load dll into target process. Last error: %lu.\n", GetLastError());
        return (int) GetLastError();
    }

    WaitForSingleObject(hRemoteThread, INFINITE);

    printf("Press any button to exit...");
    getchar();
    return 0;
}
/*
gcc -o injector.exe injector.c
 * */