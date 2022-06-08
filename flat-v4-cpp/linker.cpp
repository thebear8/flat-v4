#include "linker.hpp"

Linker::Linker() :
	rawAddress(0),
	virtualAddress(0),
	isLayoutPass(false)
{
}

Linker& Linker::beginPass(bool isLayoutPass)
{
	buffer.clear();
	rawAddress = 0;
	virtualAddress = 0;
	this->isLayoutPass = isLayoutPass;
	return *this;
}

Linker& Linker::symbol(std::string const& name)
{
	if(isLayoutPass)
		symbols.try_emplace(name, std::pair<size_t, size_t>(rawAddress, virtualAddress));
	return *this;
}

size_t Linker::getSymbol(std::string const& name)
{
	if (!isLayoutPass)
		return symbols.at(name).second;
	return 0;
}

size_t Linker::getSymbolRaw(std::string const& name)
{
	if (!isLayoutPass)
		return symbols.at(name).first;
	return 0;
}

Linker& Linker::align(size_t rawAlign, size_t virtualAlign)
{
	size_t alignedRawAddress = std::max((size_t)1, rawAlign) * ceil((double)rawAddress / std::max((size_t)1, rawAlign));
	size_t alignedVirtualAddress = std::max((size_t)1, virtualAlign) * ceil((double)virtualAddress / std::max((size_t)1, virtualAlign));
	buffer.fill('\0', alignedRawAddress - rawAddress);
	rawAddress = alignedRawAddress;
	virtualAddress = alignedVirtualAddress;
	return *this;
}

size_t Linker::calculateAlignedValue(size_t value, size_t alignment)
{
	return std::max((size_t)1, alignment) * ceil((double)value / (double)std::max((size_t)1, alignment));
}