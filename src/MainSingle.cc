#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

int
main()
{
	Image img;
	Tracer{Scene{0}, &img}.traceAntialiased();
}
