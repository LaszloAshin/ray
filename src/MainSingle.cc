#include "Targa.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

int
main()
{
	Targa img(WIDTH, HEIGHT);
	Tracer{Scene{0}, &img}.exec("tracement.tga");
}
