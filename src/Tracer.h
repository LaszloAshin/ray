#pragma once

#include "Image.h"
#include "Scene.h"

template <int BlockSize>
struct Blocks {
	int x_blocks, all_blocks;
	volatile int next_block{};

	Blocks(const Image& img)
	: x_blocks{(img.getWidth() + BlockSize - 1) / BlockSize}
	, all_blocks{((img.getHeight() + BlockSize - 1) / BlockSize) * x_blocks}
	{}

	struct Block {
		int x0, y0;
		int x1, y1;

		Block(const Blocks& blocks, const Image& img, int i)
		: x0{(i % blocks.x_blocks) * BlockSize}
		, y0{(i / blocks.x_blocks) * BlockSize}
		, x1{std::min(x0 + BlockSize, img.getWidth())}
		, y1{std::min(y0 + BlockSize, img.getHeight())}
		{}

		operator bool() const { return x0 < x1 && y0 < y1; }
	};

	void reset() { next_block = 0; }

	Block next(const Image& img) {
		int result = next_block;
		next_block = std::min(all_blocks, next_block + 1);
		return Block{*this, img, result};
	}

	float progress() const { return static_cast<float>(next_block) / all_blocks; }

	static constexpr int blockSize() { return BlockSize; }
};

struct Tracer {
	Tracer(const Scene &s, Image *i)
	: scene{s}
	, img{i}
	, blocks{*i}
	, invwhalf{1.0f / (i->getWidth() / 2.0f)}
	, hhalf{i->getHeight() / 2.0f}
	{}

	void exec(const char *fname, bool turbo=false);

protected:
	using MyBlocks = Blocks<32>;

	virtual MyBlocks::Block getNextBlock();
	virtual void consumeBlocks(bool turbo);

private:
	Vec3f viewVec(int x0, int y0, float dx, float dy) const;
	void turboTracer();
	void blockTracer();

	const Scene &scene;
	Image *img;
	MyBlocks blocks;
	float invwhalf;
	float hhalf;
};
