#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

template <typename T, int Capacity>
struct Vector {
	Vector() {}

	~Vector() {
		while (size_ > 0) {
			values[--size_].~T();
		}
	}

	template <typename U>
	void push_back(U&& value) {
		assert(size_ < Capacity);
		new(values + size_) T(std::forward<U>(value));
		++size_;
	}

	template <typename... Args>
	T& emplace_back(Args&&... args) {
		assert(size_ < Capacity);
		new(values + size_) T(std::forward<Args>(args)...);
		return values[size_++];
	}

	struct ConstIterator {
		bool operator!=(const ConstIterator& rhs) const { return p != rhs.p; }
		ConstIterator& operator++() { ++p; return *this; }
		const T& operator*() { return *p; }

	private:
		friend class Vector;

		ConstIterator(const T* p) : p{p} {}

		const T* p;
	};

	ConstIterator begin() const { return ConstIterator{values}; }
	ConstIterator end() const { return ConstIterator{values + size_}; }
	size_t size() const { return size_; }
	const T& operator[](int i) const { return values[i]; }
	T& operator[](int i) { return values[i]; }

private:
	int size_{};
	union {
		char dummy;
		T values[Capacity];
	};
};

