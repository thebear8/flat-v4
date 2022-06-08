#include "pe_generator.hpp"
#include <Windows.h>

PeGenerator::PeGenerator(Linker& ctx) :
	ctx(ctx)
{
	addSection(".code", "__code_begin", "__code_end", IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ);
	addSection(".data", "__data_begin", "__data_end", IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
	addSection(".idata", "__idata_begin", "__idata_end", IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
}

void PeGenerator::addSection(std::string const& name, std::string const& begin, std::string const& end, size_t characteristics)
{
	sections[name] = std::make_tuple(begin, end, characteristics);
}

void PeGenerator::addImport(std::string const& dll, std::string const& function)
{
	if (!imports.contains(dll))
		imports.try_emplace(dll, std::vector<std::string>());
	imports.at(dll).push_back(function);
}

void PeGenerator::beginImage()
{
	ctx.symbol("__image_begin");
}

void PeGenerator::endImage()
{
	ctx.symbol("__image_end");
}

void PeGenerator::writePeHeader()
{
	IMAGE_DOS_HEADER dosHeader = {};
	dosHeader.e_magic = IMAGE_DOS_SIGNATURE;
	dosHeader.e_lfanew = 0x100;
	ctx.push(dosHeader);
	ctx.align(0x100, 0x100);

	IMAGE_NT_HEADERS64 ntHeader = {};
	ntHeader.Signature = IMAGE_NT_SIGNATURE;
	ntHeader.FileHeader.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_DEBUG_STRIPPED | IMAGE_FILE_LARGE_ADDRESS_AWARE;
	ntHeader.FileHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
	ntHeader.FileHeader.NumberOfSections = 3;
	ntHeader.FileHeader.NumberOfSymbols = 0;
	ntHeader.FileHeader.PointerToSymbolTable = 0;
	ntHeader.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);
	ntHeader.FileHeader.TimeDateStamp = time(nullptr);

	ntHeader.OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
	ntHeader.OptionalHeader.MajorLinkerVersion = 0;
	ntHeader.OptionalHeader.MinorLinkerVersion = 1;
	ntHeader.OptionalHeader.SizeOfCode = ctx.getSymbol("__code_end") - ctx.getSymbol("__code_begin");
	ntHeader.OptionalHeader.SizeOfInitializedData = (ctx.getSymbol("__data_end") - ctx.getSymbol("__data_begin")) + (ctx.getSymbol("__idata_end") - ctx.getSymbol("__idata_begin"));
	ntHeader.OptionalHeader.SizeOfUninitializedData = 0;
	ntHeader.OptionalHeader.AddressOfEntryPoint = ctx.getSymbol("__entry");
	ntHeader.OptionalHeader.BaseOfCode = ctx.getSymbol("__code_begin");
	ntHeader.OptionalHeader.ImageBase = 0x140000000;
	ntHeader.OptionalHeader.SectionAlignment = SECTION_ALIGNMENT;
	ntHeader.OptionalHeader.FileAlignment = FILE_ALIGNMENT;
	ntHeader.OptionalHeader.MajorOperatingSystemVersion = 6;
	ntHeader.OptionalHeader.MinorOperatingSystemVersion = 0;
	ntHeader.OptionalHeader.MajorImageVersion = 0;
	ntHeader.OptionalHeader.MinorImageVersion = 0;
	ntHeader.OptionalHeader.MajorSubsystemVersion = 6;
	ntHeader.OptionalHeader.MinorSubsystemVersion = 0;
	ntHeader.OptionalHeader.Win32VersionValue = 0;
	ntHeader.OptionalHeader.SizeOfImage = ctx.getSymbol("__image_end") - ctx.getSymbol("__image_begin");
	ntHeader.OptionalHeader.SizeOfHeaders = ctx.getSymbol("__headers_end");
	ntHeader.OptionalHeader.CheckSum = 0;
	ntHeader.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
	ntHeader.OptionalHeader.DllCharacteristics = IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE | IMAGE_DLLCHARACTERISTICS_NX_COMPAT | IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE;
	ntHeader.OptionalHeader.SizeOfStackReserve = 0x10000;
	ntHeader.OptionalHeader.SizeOfStackCommit = 0x10000;
	ntHeader.OptionalHeader.SizeOfHeapReserve = 0x10000;
	ntHeader.OptionalHeader.SizeOfHeapCommit = 0x10000;
	ntHeader.OptionalHeader.LoaderFlags = 0;
	ntHeader.OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

	if (imports.size() > 0)
	{
		ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = ctx.getSymbol("__idata_begin");
		ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = ctx.getSymbol("__idata_end") - ctx.getSymbol("__idata_begin");
	}

	ctx.push(ntHeader);
}

void PeGenerator::writePeSectionHeaders()
{
	auto sections = this->sections;
	while (sections.size())
	{
		std::string name;
		for (auto& section : sections)
		{
			if (name.empty() || ctx.getSymbol(std::get<0>(sections.at(section.first))) < ctx.getSymbol(std::get<0>(sections.at(name))))
				name = section.first;
		}

		auto& [begin, end, characteristics] = sections[name];
		IMAGE_SECTION_HEADER header = {};
		header.Characteristics = characteristics;
		header.PointerToRawData = ctx.getSymbolRaw(begin);
		header.SizeOfRawData = ctx.calculateAlignedValue(ctx.getSymbolRaw(end) - ctx.getSymbolRaw(begin), FILE_ALIGNMENT);
		header.VirtualAddress = ctx.getSymbol(begin);
		header.Misc.VirtualSize = ctx.getSymbol(end) - ctx.getSymbol(begin);
		std::copy(name.c_str(), name.c_str() + std::min<size_t>(name.length(), 8), header.Name);
		ctx.push(header);

		sections.erase(name);
	}

	ctx.symbol("__headers_end");
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
}

void PeGenerator::writePeImportSection()
{
	ctx.symbol("__idata_begin");

	// -------------------------------------- import directory table table

	for (auto& [dll, functions] : imports)
	{
		IMAGE_IMPORT_DESCRIPTOR desc = {};
		desc.Name = ctx.getSymbol("__" + dll + "_name");
		desc.FirstThunk = ctx.getSymbol("__" + dll + "_import_address_table");
		desc.OriginalFirstThunk = ctx.getSymbol("__" + dll + "_import_lookup_table");
		ctx.push(desc);
	}

	ctx.push(IMAGE_IMPORT_DESCRIPTOR());

	// -------------------------------------- import lookup/address table

	for (auto& [dll, functions] : imports)
	{
		ctx.symbol("__" + dll + "_import_lookup_table");
		for (auto& function : functions)
		{
			IMAGE_IMPORT_LOOKUP_TABLE_ENTRY64 entry = {};
			entry.HintNameTableRVA = ctx.getSymbol("__" + function + "_name_table_entry");
			ctx.push(entry);
		}
		ctx.push(IMAGE_IMPORT_LOOKUP_TABLE_ENTRY64());

		ctx.symbol("__" + dll + "_import_address_table");
		for (auto& function : functions)
		{
			ctx.symbol("__imp_" + function);
			IMAGE_IMPORT_LOOKUP_TABLE_ENTRY64 entry = {};
			entry.HintNameTableRVA = ctx.getSymbol("__" + function + "_name_table_entry");
			ctx.push(entry);
		}
		ctx.push(IMAGE_IMPORT_LOOKUP_TABLE_ENTRY64());
	}

	// -------------------------------------- hint/name table

	for (auto& [dll, functions] : imports)
	{
		for (auto& function : functions)
		{
			ctx.symbol("__" + function + "_name_table_entry");
			ctx.push<uint16_t>(0);
			ctx.push(function.c_str(), function.size() + 1);
		}
	}

	ctx.align(2, 2);

	// -------------------------------------- dll names

	for (auto& [dll, functions] : imports)
	{
		ctx.symbol("__" + dll + "_name");
		ctx.push(dll.c_str(), dll.size() + 1);
	}

	// -------------------------------------- end section

	ctx.symbol("__idata_end");
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
}

void PeGenerator::beginSection(std::string const& name)
{
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
	ctx.symbol(name + "_begin");
}

void PeGenerator::endSection(std::string const& name)
{
	ctx.symbol(name + "_end");
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
}

void PeGenerator::beginPeDataSection()
{
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
	ctx.symbol("__data_begin");
}

void PeGenerator::endPeDataSection()
{
	ctx.symbol("__data_end");
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
}

void PeGenerator::beginPeCodeSection()
{
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
	ctx.symbol("__code_begin");
}

void PeGenerator::endPeCodeSection()
{
	ctx.symbol("__code_end");
	ctx.align(FILE_ALIGNMENT, SECTION_ALIGNMENT);
}