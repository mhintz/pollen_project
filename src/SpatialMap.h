#pragma once

#include <map>
#include <unordered_set>
#include <cstdint>
#include <exception>

inline uint32_t cantorPair(uint32_t a, uint32_t b) {
	return ((a + b) * (a + b + 1) / 2) + b;
}

inline uint32_t cantorHash(uint32_t a, uint32_t b, uint32_t c) {
	return cantorPair(cantorPair(a, b), c);
}

class ExcOutOfRange : public std::exception {
};

template <typename T>
class SpatialMap {
public:
	struct IntGridPos {
		IntGridPos(int inX, int inY, int inZ) : x(inX), y(inY), z(inZ) {}
		int x;
		int y;
		int z;
	};

	typedef IntGridPos (*AccessorFn) (T);

	inline uint32_t hash(int x, int y, int z) {
		int relX = x - mXLo;
		int relY = y - mYLo;
		int relZ = z - mZLo;
		if (relX < 0 || relY < 0 || relZ < 0) { throw ExcOutOfRange(); }
		return cantorHash(relX, relY, relZ);
	}

	SpatialMap(AccessorFn acc, int lox, int loy, int loz) : mAccessor(acc), mXLo(lox), mYLo(loy), mZLo(loz) {}

	bool add(T value) {
		IntGridPos valuePos = mAccessor(value);
		uint32_t hashVal = hash(valuePos.x, valuePos.y, valuePos.z);
		if (!mStorage.count(hashVal)) {
			mStorage[hashVal] = std::unordered_set<T>();
		}
		auto result = mStorage[hashVal].insert(value);
		return result.second;
	}

	bool has(T value) {
		IntGridPos valuePos = mAccessor(value);
		uint32_t hashVal = hash(valuePos.x, valuePos.y, valuePos.z);
		return (bool) mStorage.count(hashVal) && (bool) mStorage[hashVal].count(value);
	}

	void remove(T value) {
		IntGridPos valuePos = mAccessor(value);
		uint32_t hashVal = hash(valuePos.x, valuePos.y, valuePos.z);
		if (mStorage.count(hashVal)) {
			mStorage[hashVal].erase(value);
		}
	}

	std::unordered_set<T> get(IntGridPos pos) {
		uint32_t hashVal = hash(pos.x, pos.y, pos.z);
		if (mStorage.count(hashVal)) {
			return mStorage[hashVal];
		} else {
			return std::unordered_set<T>();
		}
	}

private:
	std::map<int, std::unordered_set<T>> mStorage;
	AccessorFn mAccessor;
	int mXLo = 0, mYLo = 0, mZLo = 0;
};