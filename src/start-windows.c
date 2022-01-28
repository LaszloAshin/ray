#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main();

int mainCRTStartup() {
       ExitProcess(main());
}
