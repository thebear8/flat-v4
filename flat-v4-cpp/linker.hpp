#pragma once
#include <string>
#include <algorithm>
#include <unordered_map>

#include "blob.hpp"

#undef min
#undef max

class Linker
{
private:
	Blob buffer;
	size_t rawAddress, virtualAddress;
	std::unordered_map<std::string, std::pair<size_t, size_t>> symbols;
	bool isLayoutPass;

public:
	Linker();

public:
	Linker& beginPass(bool layoutPass);

	Linker& symbol(std::string const& name);
	size_t getSymbol(std::string const& name);
	size_t getSymbolRaw(std::string const& name);

	inline size_t getCurrentAddress() { return virtualAddress; }
	inline size_t getCurrentAddressRaw() { return rawAddress; }

	inline Blob const& getData() { return buffer; }

	Linker& align(size_t rawAlign, size_t virtualAlign);
	size_t calculateAlignedValue(size_t value, size_t alignment);

	template<typename T>
	Linker& push(T const& value)
	{
		rawAddress += sizeof(value);
		virtualAddress += sizeof(value);
		buffer.append(value);
		return *this;
	}

	template<>
	Linker& push(Blob const& value)
	{
		rawAddress += value.size();
		virtualAddress += value.size();
		buffer.append(value);
		return *this;
	}

	template<typename T>
	Linker& push(T const* values, size_t count)
	{
		rawAddress += count * sizeof(T);
		virtualAddress += count * sizeof(T);
		buffer.append(values, count);
		return *this;
	}

	template<typename T>
	Linker& operator<<(T const& value)
	{
		return push(value);
	}
};