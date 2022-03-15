#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

int
main(int, char*[], char* envp[])
{
	int width = WIDTH;
	if (const char* p = mygetenv(envp, "WIDTH")) {
		width = myatoi(p);
	}
	int height = HEIGHT;
	if (const char* p = mygetenv(envp, "HEIGHT")) {
		height = myatoi(p);
	}
	Image img(width, height);
	Tracer{Scene{0}, &img}.traceAntialiased();
}
