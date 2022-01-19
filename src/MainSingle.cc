#include "Image.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

int
main()
{
	int width = WIDTH;
	if (const char* p = std::getenv("WIDTH")) {
		width = atoi(p);
	}
	int height = HEIGHT;
	if (const char* p = std::getenv("HEIGHT")) {
		height = atoi(p);
	}
	Image img(width, height);
	Tracer{Scene{0}, &img}.exec("tracement.ppm");
}
