#pragma once
#include <map>

#include "linker.hpp"
#include "literals.hpp"

struct IMAGE_IMPORT_LOOKUP_TABLE_ENTRY64
{
	union
	{
		uint16_t Ordinal : 16;
		uint32_t HintNameTableRVA : 31;
	};

	uint32_t __Padding : 31;
	uint32_t OrdinalNameFlag : 1;
};

class PeGenerator
{
	static constexpr size_t FILE_ALIGNMENT = 0x200;
	static constexpr size_t SECTION_ALIGNMENT = 0x1000;

private:
	Linker& ctx;
	std::unordered_map<std::string, std::tuple<std::string, std::string, size_t>> sections;
	std::unordered_map<std::string, std::vector<std::string>> imports;

public:
	PeGenerator(Linker& ctx);

	void addSection(std::string const& name, std::string const& begin, std::string const& end, size_t characteristics);
	void addImport(std::string const& dll, std::string const& function);

	void beginImage();
	void endImage();

	void writePeHeader();
	void writePeSectionHeaders();
	void writePeImportSection();

	void beginSection(std::string const& name);
	void endSection(std::string const& name);

	void beginPeDataSection();
	void endPeDataSection();

	void beginPeCodeSection();
	void endPeCodeSection();
};