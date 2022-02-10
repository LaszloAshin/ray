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

Vec3f Tracer::viewVec(int x0, int y0, float dx, float dy) const {
	float x = dx + x0;
	float y = dy + y0;
	return Vec3f{x * invwhalf - 1.0f, (hhalf - y) * invwhalf, -0.5f}.norm();
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
				Ray r(Vec3f{}, viewVec(x, y, 0.5f, 0.5f));
				img->setPixel(x, y, scene.trace(r));
			}
		}
	}
}

Color Tracer::computePixel(int x, int y, Color* up, Color& left) {
	Color right = scene.trace(Ray{Vec3f{}, viewVec(x, y, 1.0f, 1.0f)});

	Color c{};
	if ((up[0].dist(right) < 0.001f) && (up[1].dist(left) < 0.001f)) {
		c = (up[0] + up[1] + left + right) * 0.25f;
	} else {
		for (int j = 0; j < SAMPLES; ++j) {
			float hx = halton(2, j + 1);
			float hy = halton(3, j + 1);
			c += scene.trace(Ray{Vec3f{}, viewVec(x, y, hx, hy)});
		}
		c = c * (1.0f / SAMPLES);
	}
	up[0] = left;
	left = right;
	return c;
}

void
Tracer::traceAntialiased()
{
	int block;

	while ((block = getNextBlock()) < blocks.all_blocks) {
		const auto b = blocks.get(*img, block);

		Color up[MyBlocks::blockSize() + 1];
		for (int x = b.x0, i = 0; x <= b.x1; ++x, ++i) {
			Ray r{Vec3f{}, viewVec(x, b.y0, 0.0f, 0.0f)};
			up[i] = scene.trace(r);
		}
		for (int y = b.y0; y < b.y1; ++y) {
			Ray r{Vec3f{}, viewVec(b.x0, y, 0.0f, 1.0f)};
			Color left = scene.trace(r);
			int i = 0;
			for (int x = b.x0; x < b.x1; ++x, ++i) {
				img->setPixel(x, y, computePixel(x, y, up + i, left));
			}
			up[i] = left;
		}
	}
}
