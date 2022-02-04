#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main(int argc, char* argv[], char* envp[]);

int mainCRTStartup() {
       ExitProcess(main(0, NULL, NULL));
}
