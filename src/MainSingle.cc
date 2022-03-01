#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"

#include "config.h"

static char scene_buffer[sizeof(Scene)];

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
	Image img("tracement.ppm", width, height);
	Scene* scene = reinterpret_cast<Scene*>(scene_buffer);
	new(scene) Scene{0};
	Tracer{*scene, &img}.traceAntialiased();
}
