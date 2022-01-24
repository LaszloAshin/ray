#define _CRT_SECURE_NO_WARNINGS
#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

int
main()
{
	int width = WIDTH;
	if (const char* p = std::getenv("WIDTH")) {
		width = myatoi(p);
	}
	int height = HEIGHT;
	if (const char* p = std::getenv("HEIGHT")) {
		height = myatoi(p);
	}
	Image img("tracement.ppm", width, height);
	Tracer{Scene{0}, &img}.exec();
}
