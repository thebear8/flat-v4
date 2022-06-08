#pragma once
#include <cstdint>

#include "linker.hpp"

namespace x64
{
	enum Reg
	{
		RAX = 0,
		RCX = 1,
		RDX = 2,
		RBX = 3,
		RSP = 4,
		RBP = 5,
		RSI = 6,
		RDI = 7,
		R8 = 8,
		R9 = 9,
		R10 = 10,
		R11 = 11,
		R12 = 12,
		R13 = 13,
		R14 = 14,
		R15 = 15,
	};
}

class CodeGenerator
{
private:
	Linker& ctx;

public:
	CodeGenerator(Linker& ctx);

public:
	void generateProlog(size_t numParameters, size_t stackSpace);
	void generateEpilog(size_t numParameters, size_t stackSpace);

public:
	uint8_t modRm(uint8_t mod, uint8_t reg, uint8_t rm);
	uint8_t sib(uint8_t scale, uint8_t idx, uint8_t base);
	uint8_t rex(uint8_t w, uint8_t reg, uint8_t idx, uint8_t rm);

	void emitPush(uint8_t reg);
	void emitPop(uint8_t reg);

	void emitAddRIm8(uint8_t reg, uint8_t value);
	void emitAddRIm32(uint8_t reg, uint32_t value);
	void emitAddRR(uint8_t reg1, uint8_t reg2);

	void emitSubRIm8(uint8_t reg, uint8_t value);
	void emitSubRIm32(uint8_t reg, uint32_t value);
	void emitSubRR(uint8_t reg1, uint8_t reg2);

	void emitMulR(uint8_t reg);
	void emitDivR(uint8_t reg);
	void emitIMulR(uint8_t reg);
	void emitIDivR(uint8_t reg);

	void emitNegR(uint8_t reg);
	void emitNotR(uint8_t reg);

	void emitCmpRR(uint8_t reg1, uint8_t reg2);
	void emitTestRR(uint8_t reg1, uint8_t reg2);

	void emitMovRR(uint8_t reg1, uint8_t reg2);
	void emitMovRIm64(uint8_t reg, uint64_t value);
	void emitMovRel8R(uint8_t reg1, int8_t offset, uint8_t reg2);
	void emitMovRel32R(uint8_t reg1, int32_t offset, uint8_t reg2);
	void emitMovRRel8(uint8_t reg1, uint8_t reg2, int8_t offset);
	void emitMovRRel32(uint8_t reg1, uint8_t reg2, int32_t offset);

	void emitAndRR(uint8_t reg1, uint8_t reg2);

	void emitOrRR(uint8_t reg1, uint8_t reg2);

	void emitXorRR(uint8_t reg1, uint8_t reg2);
	void emitXorRIm8(uint8_t reg, uint8_t value);
	void emitXorRIm32(uint8_t reg, uint32_t value);

	void emitShlRCl(uint8_t reg);
	void emitShrRCl(uint8_t reg);

	void emitSetE(uint8_t reg);
	void emitSetNe(uint8_t reg);
	void emitSetL(uint8_t reg);
	void emitSetG(uint8_t reg);
	void emitSetLe(uint8_t reg);
	void emitSetGe(uint8_t reg);

	void emitLeaRRipRel32(uint8_t reg, size_t address);

	void emitCallRipRel32(size_t address);
	void emitCallPtrRipRel32(size_t address);

	void emitJmp(size_t address);
	void emitJmpZ(size_t address);
	void emitJmpNZ(size_t address);

	void emitNop();

	void emitReturn();
};

/*
	void generateMovC(size_t rDST, uint64_t cSRC);
	void generateMovR(size_t rDST, size_t rSRC);

	void generateLd8(size_t rDST, size_t rSRC, int32_t offset);
	void generateLd16(size_t rDST, size_t rSRC, int32_t offset);
	void generateLd32(size_t rDST, size_t rSRC, int32_t offset);
	void generateLd64(size_t rDST, size_t rSRC, int32_t offset);

	void generateSt8(size_t rDST, int32_t offset, size_t rSRC);
	void generateSt16(size_t rDST, int32_t offset, size_t rSRC);
	void generateSt32(size_t rDST, int32_t offset, size_t rSRC);
	void generateSt64(size_t rDST, int32_t offset, size_t rSRC);

	void generateAdd(size_t rDST, size_t rSRC);
	void generateSub(size_t rDST, size_t rSRC);
	void generateMul(size_t rDST, size_t rSRC);
	void generateDiv(size_t rDST, size_t rSRC);

	void generateCall(std::string const& label, std::vector<size_t> const& args);
	void generateCallReg(size_t rPTR, std::vector<size_t> const& args);
	void generateJmpEq(std::string const& label, size_t rA, size_t rB);
	void generateJmpZ(std::string const& label, size_t rA);
	void generateJmpNz(std::string const& label, size_t rA);
	void generateRet();
*/