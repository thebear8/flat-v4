#pragma once
#include <memory>

class Blob
{
private:
	void* buffer;
	size_t count, capacity;

public:
	Blob() :
		buffer(nullptr),
		count(0),
		capacity(0)
	{
	}

	Blob(size_t capacity)
	{
		expand(capacity);
	}

public:
	template<typename T>
	inline Blob& append(T const& value)
	{
		if ((count + sizeof(value)) > capacity && !expand(count + sizeof(value)))
			throw std::exception("out of memory");

		memcpy((void*)((size_t)buffer + count), &value, sizeof(value));
		count += sizeof(value);
		return *this;
	}

	template<>
	inline Blob& append<Blob>(Blob const& blob)
	{
		if ((count + blob.count) > capacity && !expand(count + blob.count))
			throw std::exception("out of memory");

		memcpy((void*)((size_t)buffer + count), blob.buffer, blob.count);
		count += blob.count;
		return *this;
	}

	template<typename T>
	inline Blob& append(T const* values, size_t count)
	{
		if ((this->count + count * sizeof(T)) > capacity && !expand(this->count + count * sizeof(T)))
			throw std::exception("out of memory");

		for (size_t i = 0; i < count; i++)
			append(values[i]);
		return *this;
	}

	template<typename T>
	inline Blob& fill(T const& value, size_t count)
	{
		if ((this->count + count * sizeof(value)) > capacity && !expand(this->count + count * sizeof(value)))
			throw std::exception("out of memory");

		for (size_t i = 0; i < count; i++)
			((T*)((size_t)buffer + this->count))[i] = value;
		this->count += count * sizeof(value);
		return *this;
	}

	inline bool expand(size_t size)
	{
		size_t newCapacity = 0x01;
		while (newCapacity < size)
			newCapacity <<= 1;

		auto newBuffer = realloc(buffer, newCapacity);
		if (newBuffer)
		{
			buffer = newBuffer;
			capacity = newCapacity;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline Blob& clear()
	{
		count = 0;
		return *this;
	}

public:
	inline void* data() const { return buffer; }
	inline size_t size() const { return count; }
};