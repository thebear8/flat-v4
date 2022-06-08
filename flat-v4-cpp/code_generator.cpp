#include "code_generator.hpp"
#include "literals.hpp"

CodeGenerator::CodeGenerator(Linker& ctx) :
	ctx(ctx)
{
}

void CodeGenerator::generateProlog(size_t numParameters, size_t stackSpace)
{
	emitMovRel8R(x64::RSP, 0x20, x64::R9);
	emitMovRel8R(x64::RSP, 0x18, x64::R8);
	emitMovRel8R(x64::RSP, 0x10, x64::RDX);
	emitMovRel8R(x64::RSP, 0x08, x64::RCX);

	emitPush(x64::RDI);
	emitPush(x64::RSI);
	emitPush(x64::RBX);
	emitPush(x64::RBP);
	emitPush(x64::R12);
	emitPush(x64::R13);
	emitPush(x64::R14);
	emitPush(x64::R15);

	size_t alignedStackSpace = stackSpace;
	if (!(std::max<size_t>(4, numParameters) % 2))
		alignedStackSpace += 8;
	emitSubRIm32(x64::RSP, alignedStackSpace);
}

void CodeGenerator::generateEpilog(size_t numParameters, size_t stackSpace)
{
	size_t alignedStackSpace = stackSpace;
	if (!(std::max<size_t>(4, numParameters) % 2))
		alignedStackSpace += 8;
	emitAddRIm32(x64::RSP, alignedStackSpace);

	emitPop(x64::R15);
	emitPop(x64::R14);
	emitPop(x64::R13);
	emitPop(x64::R12);
	emitPop(x64::RBP);
	emitPop(x64::RBX);
	emitPop(x64::RSI);
	emitPop(x64::RDI);

	emitReturn();
}

uint8_t CodeGenerator::modRm(uint8_t mod, uint8_t reg, uint8_t rm)
{
	return (uint8_t)(((mod & 0x03) << 6) | ((reg & 0x07) << 3) | ((rm & 0x07) << 0));
}

uint8_t CodeGenerator::sib(uint8_t scale, uint8_t idx, uint8_t base)
{
	return (uint8_t)(((scale & 0x03) << 6) | ((idx & 0x07) << 3) | ((base & 0x07) << 0));
}

uint8_t CodeGenerator::rex(uint8_t w, uint8_t reg, uint8_t idx, uint8_t rm)
{
	return (uint8_t)(0x40 | ((w & 0x01) << 3) | ((!!(reg & 0x08)) << 2) | ((!!(idx & 0x08)) << 1) | ((!!(rm & 0x08)) << 0));
}

void CodeGenerator::emitPush(uint8_t reg)
{
	ctx << rex(1, reg, 0, 0) << (uint8_t)(0x50 | (reg & 0x07));
}

void CodeGenerator::emitPop(uint8_t reg)
{
	ctx << rex(1, reg, 0, 0) << (uint8_t)(0x58 | (reg & 0x07));
}

void CodeGenerator::emitAddRIm8(uint8_t reg, uint8_t value)
{
	ctx << rex(1, reg, 0, 0) << 0x83uss << modRm(0x03, 0x00, reg) << value;
}

void CodeGenerator::emitAddRIm32(uint8_t reg, uint32_t value)
{
	ctx << rex(1, reg, 0, 0) << 0x81uss << modRm(0x03, 0x00, reg) << value;
}

void CodeGenerator::emitAddRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x01uss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitSubRIm8(uint8_t reg, uint8_t value)
{
	ctx << rex(1, reg, 0, 0) << 0x83uss << modRm(0x03, 0x05, reg) << value;
}

void CodeGenerator::emitSubRIm32(uint8_t reg, uint32_t value)
{
	ctx << rex(1, reg, 0, 0) << 0x81uss << modRm(0x03, 0x05, reg) << value;
}

void CodeGenerator::emitSubRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x29uss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitMulR(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xF7uss << modRm(0x03, 0x04, reg);
}

void CodeGenerator::emitDivR(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xF7uss << modRm(0x03, 0x06, reg);
}

void CodeGenerator::emitIMulR(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xF7uss << modRm(0x03, 0x05, reg);
}

void CodeGenerator::emitIDivR(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xF7uss << modRm(0x03, 0x07, reg);
}

void CodeGenerator::emitNegR(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xF7uss << modRm(0x03, 0x03, reg);
}

void CodeGenerator::emitNotR(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xF7uss << modRm(0x03, 0x02, reg);
}

void CodeGenerator::emitTestRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x85uss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitCmpRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x39ss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitMovRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x89uss << modRm(0x11, reg1, reg2);
}

void CodeGenerator::emitMovRIm64(uint8_t reg, uint64_t value)
{
	ctx << rex(1, reg, 0, 0) << (uint8_t)(0xB8uss | (reg & 0x07)) << value;
}

void CodeGenerator::emitMovRel8R(uint8_t reg1, int8_t offset, uint8_t reg2)
{
	ctx << rex(1, reg2, 0, reg1) << 0x89uss << modRm(0x01, reg2, 0x04) << sib(0x00, 0x04, reg1) << offset;
}

void CodeGenerator::emitMovRel32R(uint8_t reg1, int32_t offset, uint8_t reg2)
{
	ctx << rex(1, reg2, 0, reg1) << 0x89uss << modRm(0x02, reg2, 0x04) << sib(0x00, 0x04, reg1) << offset;
}

void CodeGenerator::emitMovRRel8(uint8_t reg1, uint8_t reg2, int8_t offset)
{
	ctx << rex(1, reg1, 0, 0) << 0x8Buss << modRm(0x01, reg1, 0x04) << sib(0x00, 0x04, reg2) << offset;
}

void CodeGenerator::emitMovRRel32(uint8_t reg1, uint8_t reg2, int32_t offset)
{
	ctx << rex(1, reg1, 0, 0) << 0x8Buss << modRm(0x02, reg2, 0x04) << sib(0x00, 0x04, reg2) << offset;
}

void CodeGenerator::emitAndRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x23uss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitOrRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x0Buss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitXorRR(uint8_t reg1, uint8_t reg2)
{
	ctx << rex(1, reg1, 0, reg2) << 0x33uss << modRm(0x03, reg1, reg2);
}

void CodeGenerator::emitXorRIm8(uint8_t reg, uint8_t value)
{
	ctx << rex(0, 0, 0, reg) << 0x83uss << modRm(0x03, 0x06, reg) << value;
}

void CodeGenerator::emitXorRIm32(uint8_t reg, uint32_t value)
{
	ctx << rex(1, 0, 0, reg) << 0x81uss << modRm(0x03, 0x06, reg) << value;
}

void CodeGenerator::emitShlRCl(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xD3 << modRm(0x03, 0x04, reg);
}

void CodeGenerator::emitShrRCl(uint8_t reg)
{
	ctx << rex(1, 0, 0, reg) << 0xD3 << modRm(0x03, 0x05, reg);
}

void CodeGenerator::emitSetE(uint8_t reg)
{
	ctx << 0x0Fuss << 0x94uss << modRm(0x03, 0, reg);
}

void CodeGenerator::emitSetNe(uint8_t reg)
{
	ctx << 0x0Fuss << 0x95uss << modRm(0x03, 0, reg);
}

void CodeGenerator::emitSetL(uint8_t reg)
{
	ctx << 0x0Fuss << 0x9Cuss << modRm(0x03, 0, reg);
}

void CodeGenerator::emitSetG(uint8_t reg)
{
	ctx << 0x0Fuss << 0x9Fuss << modRm(0x03, 0, reg);
}

void CodeGenerator::emitSetLe(uint8_t reg)
{
	ctx << 0x0Fuss << 0x9Euss << modRm(0x03, 0, reg);
}

void CodeGenerator::emitSetGe(uint8_t reg)
{
	ctx << 0x0Fuss << 0x9Duss << modRm(0x03, 0, reg);
}

void CodeGenerator::emitLeaRRipRel32(uint8_t reg, size_t address)
{
	ctx << rex(1, reg, 0, 0) << 0x8Duss << modRm(0x00, reg, 0x05) << (int32_t)(address - (ctx.getCurrentAddress() + 4));
}

void CodeGenerator::emitCallRipRel32(size_t address)
{
	ctx << 0xE8uss << (int32_t)(address - (ctx.getCurrentAddress() + 4));
}

void CodeGenerator::emitCallPtrRipRel32(size_t address)
{
	ctx << 0xFFuss << modRm(0x00, 0x02, 0x05) << (int32_t)(address - (ctx.getCurrentAddress() + 4));
}

void CodeGenerator::emitJmpZ(size_t address)
{
	ctx << 0x0Fuss << 0x84uss << (int32_t)(address - (ctx.getCurrentAddress() + 4));
}

void CodeGenerator::emitJmp(size_t address)
{
	ctx << 0xE9uss << (int32_t)(address - (ctx.getCurrentAddress() + 4));
}

void CodeGenerator::emitJmpNZ(size_t address)
{
	ctx << 0x0Fuss << 0x85uss << (int32_t)(address - (ctx.getCurrentAddress() + 4));
}

void CodeGenerator::emitNop()
{
	ctx << 0x90uss;
}

void CodeGenerator::emitReturn()
{
	ctx << 0xC3uss;
}