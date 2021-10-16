#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <libgen.h>

#define SLEEP_TIME 1000

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

struct Arguments {
    const char * handlerExe;
    const char * targetProcess;
};

DWORD WINAPI wait_for_target_process(void *data) {
    struct Arguments* arguments = (struct Arguments *) data;

    HANDLE process;
    HANDLE hProcessHandler;
    char commandLine[2048];

    while (TRUE) {
        printf("Waiting for target process...\n");

        while (TRUE) {
            DWORD pid = getProcessId(arguments->targetProcess);
            if (pid) {
                process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                printf("Target process detected.\n");
                sprintf(commandLine, "%s %lu", arguments->handlerExe, pid);

                STARTUPINFO startupInfo;
                ZeroMemory(&startupInfo, sizeof(startupInfo));
                startupInfo.cb = sizeof(startupInfo);

                PROCESS_INFORMATION processInfo;
                ZeroMemory(&processInfo, sizeof(processInfo));

                printf("Starting handler process...\n");
                if (!(CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))) {
                    printf("Error: %lu.\n", GetLastError());
                    return -1;
                }
                printf("Started.\n");
                hProcessHandler = processInfo.hProcess;
                break;
            } else {
                Sleep(SLEEP_TIME);
            }
        }
        printf("Waiting for process shutdown...\n");
        WaitForSingleObject(process, INFINITE);
        printf("Target process shutdown, terminating handler process...\n");
        TerminateProcess(hProcessHandler, 0);
        printf("Handler process terminated.\n\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Too few arguments: %s <target_process> <handler_app>\n", basename(argv[0]));
        return -1;
    }

    struct Arguments * arguments = calloc(1, sizeof(struct Arguments));
    arguments->targetProcess = argv[1];
    arguments->handlerExe = argv[2];
    HANDLE waitingThread = CreateThread(0,
                                        0,
                                        wait_for_target_process,
                                        arguments,
                                        0,
                                        0);
    getchar();
    TerminateThread(waitingThread, 0);
    return 0;
}
