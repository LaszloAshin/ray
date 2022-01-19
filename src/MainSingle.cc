#include "Image.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

int
main()
{
	Image img(WIDTH, HEIGHT);
	Tracer{Scene{0}, &img}.exec("tracement.ppm");
}
