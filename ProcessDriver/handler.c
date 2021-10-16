//
// Created by vmega on 16.10.2021.
//

#include <stdio.h>
#include <windows.h>

int main(int argc, char * argv[]) {
    MessageBox(0, argv[1], "Target process has been started with pid...", MB_OK);
    return 0;
}