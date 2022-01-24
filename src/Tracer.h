#pragma once

#include "Image.h"
#include "Scene.h"

template <int BlockSize>
struct Blocks {
	int x_blocks, all_blocks;

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
	};

	Block get(const Image& img, int i) const { return Block{*this, img, i}; }
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

	void exec(bool turbo=false);

protected:
	virtual int getNextBlock();
	virtual void consumeBlocks(bool turbo);

private:
	using MyBlocks = Blocks<32>;

	Vec3f viewVec(int x0, int y0, float dx, float dy) const;
	void turboTracer();
	void blockTracer();

	const Scene &scene;
	Image *img;
	MyBlocks blocks;
	float invwhalf;
	float hhalf;
	volatile int next_block{};
};
