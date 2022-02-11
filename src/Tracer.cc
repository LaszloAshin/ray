#include "Tracer.h"

#include "MyLib.h"
#include "Vector.h"

#include "config.h"

using namespace std;

static float halton(int base, int n) {
	float ret = 0.0f;
	int b = base;

	while (n) {
		ret += (float)(n % base) / b;
		b *= base;
		n /= base;
	}
	return ret;
}

Ray Tracer::rayForPixel(float x, float y) const {
	return Ray{Vec3f{}, Vec3f{x * invwhalf - 1.0f, (hhalf - y) * invwhalf, -0.5f}.norm()};
}

int Tracer::getNextBlock() {
	const int result = next_block.fetch_add(1, std::memory_order_relaxed);
	const int percent = std::min(100, result * 100 / blocks.all_blocks);
	char buf[] = "\r   % ";
	overwrite_int(buf + 3, percent);
	mywrite(buf, 6);
	return result;
}

void
Tracer::trace()
{
	int block;

	while ((block = getNextBlock()) < blocks.all_blocks) {
		const auto b = blocks.get(*img, block);

		for (int y = b.y0; y < b.y1; ++y) {
			for (int x = b.x0; x < b.x1; ++x) {
				img->setPixel(x, y, scene.trace(rayForPixel(x + 0.5f, y + 0.5f)));
			}
		}
	}
}

void Tracer::traceAntialiased() {
	Color up[MyBlocks::blockSize() + 1];
	traceAntialiased(up);
}

void
Tracer::traceAntialiased(Color *up)
{
	int block;

	while ((block = getNextBlock()) < blocks.all_blocks) {
		const auto b = blocks.get(*img, block);

		for (int x = b.x0, i = 0; x <= b.x1; ++x, ++i) {
			up[i] = scene.trace(rayForPixel(x, b.y0));
		}
		for (int y = b.y0; y < b.y1; ++y) {
			Color left = scene.trace(rayForPixel(b.x0, y + 1));
			int i = 0;
			for (int x = b.x0; x < b.x1; ++x, ++i) {
				Color right = scene.trace(rayForPixel(x + 1, y + 1));
				
				Color c{};
				if ((up[i].dist(right) < 0.001f) && (up[i+1].dist(left) < 0.001f)) {
					c = (up[i] + up[i+1] + left + right) * 0.25f;
				} else {
					for (int j = 0; j < SAMPLES; ++j) {
						float hx = halton(2, j + 1);
						float hy = halton(3, j + 1);
						c += scene.trace(rayForPixel(x + hx, y + hy));
					}
					c = c * (1.0f / SAMPLES);
				}
				img->setPixel(x, y, c);

				up[i] = left;
				left = right;
			}
			up[i] = left;
		}

	}
}
