#include <stdio.h>
#include <windows.h>

extern int customPow(int, int);

typedef int (*DynamicPower)(int, int);

int dynamicPow(int base, int power) {
    HINSTANCE hLib;
    DynamicPower fPointer;

    hLib = LoadLibrary(TEXT("D:\\University\\WinApi\\LibrariesImport\\Shared\\cmake-build-debug\\libShared.dll"));

    if (hLib == NULL) {
        printf("DLL Loading fails");
        exit(-1);
    }

    fPointer = (DynamicPower) GetProcAddress(hLib, "dynamicPow");

    if (fPointer == NULL) {
        FreeLibrary(hLib);
        exit(-1);
    }

    int result = fPointer(base, power);
    FreeLibrary(hLib);
    return result;
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("Need two arguments: <prog_name> <base> <power>\n");
        exit(-1);
    }

    int base = atoi(argv[1]);
    int power = atoi(argv[2]);
    printf("Static import: customPow(%d, %d) -> %d\n", base, power, customPow(base, power));
    printf("Dynamic import: dynamicPow(%d, %d) -> %d\n", base + 1, power + 1, dynamicPow(base + 1, power + 1));
    return 0;
}
