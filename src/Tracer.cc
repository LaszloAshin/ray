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
	myprint("\r", percent, "% ");
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
				img->setPixel(x, y, scene.trace(r, DEPTH_LIMIT, 1.0f));
			}
		}
	}
}

void
Tracer::traceAntialiased()
{
	int block;

	while ((block = getNextBlock()) < blocks.all_blocks) {
		const auto b = blocks.get(*img, block);

		Color up[MyBlocks::blockSize() + 1];
		for (int x = b.x0, i = 0; x <= b.x1; ++x, ++i) {
			Ray r(Vec3f{}, viewVec(x, b.y0, 0.0f, 0.0f));
			up[i] = scene.trace(r, DEPTH_LIMIT, 1.0f);
		}
		for (int y = b.y0; y < b.y1; ++y) {
			Ray r(Vec3f{}, viewVec(b.x0, y, 0.0, 1.0));
			Color left = scene.trace(r, DEPTH_LIMIT, 1.0f);
			int i = 0;
			for (int x = b.x0; x < b.x1; ++x, ++i) {
				r = Ray(Vec3f{}, viewVec(x, y, 1.0, 1.0));
				Color right = scene.trace(r, DEPTH_LIMIT, 1.0f);
				
				Color c{};
				if ((up[i].dist(right) < 0.001f) && (up[i+1].dist(left) < 0.001f)) {
					c = (up[i] + up[i+1] + left + right) * 0.25f;
				} else {
					for (int j = 0; j < SAMPLES; ++j) {
						float hx = halton(2, j + 1);
						float hy = halton(3, j + 1);
						r.d = viewVec(x, y, hx, hy);
						c += scene.trace(r, DEPTH_LIMIT, 1.0f);
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
