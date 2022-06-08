#include "codegen_pass.hpp"

void CodeGenPass::generateCode(std::unordered_map<std::string, std::vector<FunctionDeclaration>> functions)
{
	for (auto& [name, cluster] : functions)
	{
		for (auto& function : cluster)
		{
			function.name += "(";
			for (size_t i = 0; i < function.parameters.size(); i++)
			{
				function.name += function.parameters[i].second->toString();
				if (i != function.parameters.size())
					function.name += ",";
			}
			function.name += ")";

			ctx.symbol(function.name);

			size_t offset = 0;
			localVariables.clear();
			for (auto& variable : function.localVariables)
			{
				localVariables.try_emplace(variable.first, std::pair(offset, variable.second));
				offset = align(offset + align(variable.second->getBitSize(), 8) / 8, typeCtx.pointerSize);
			}
		}
	}
}

void CodeGenPass::visit(IntegerExpression* node)
{
	codeGen.emitMovRIm64(x64::RAX, std::stoll(std::string(node->value)));
	codeGen.emitPush(x64::RAX);
}

void CodeGenPass::visit(IdentifierExpression* node)
{
	auto name = std::string(node->value);
	auto type = localVariables.at(name).first;
	auto offset = localVariables.at(name).second;
	auto size = align(type->getBitSize(), 8) / 8;

	if (size <= typeCtx.pointerSize)
	{
		codeGen.emitMovRRel32(x64::RAX, x64::RBP, -((int32_t)offset));
		codeGen.emitPush(x64::RAX);
	}
	else
	{
		throw std::exception("not implemented");
	}
}

void CodeGenPass::visit(UnaryExpression* node)
{
	AstVisitor::visit(node->expression.get());

	codeGen.emitPop(x64::RAX);

	if (node->type == Token::Plus)
		codeGen.emitNop();
	else if (node->type == Token::Minus)
		codeGen.emitNegR(x64::RAX);
	else if (node->type == Token::LogicalNot)
		codeGen.emitXorRIm8(x64::RAX, 0x01);
	else if (node->type == Token::BitwiseNot)
		codeGen.emitNotR(x64::RAX);
	else
		throw std::exception("invalid unary operator type");

	codeGen.emitPush(x64::RAX);
}

void CodeGenPass::visit(BinaryExpression* node)
{
	AstVisitor::visit(node->left.get());
	AstVisitor::visit(node->right.get());

	codeGen.emitPop(x64::RCX);
	codeGen.emitPop(x64::RAX);

	if (node->type == Token::Plus)
		codeGen.emitAddRR(x64::RAX, x64::RCX);
	else if (node->type == Token::Minus)
		codeGen.emitSubRR(x64::RAX, x64::RCX);
	else if (node->type == Token::Multiply)
		codeGen.emitIMulR(x64::RCX);
	else if (node->type == Token::Divide)
		codeGen.emitIDivR(x64::RCX);

	else if (node->type == Token::BitwiseAnd || node->type == Token::LogicalAnd)
		codeGen.emitAndRR(x64::RAX, x64::RCX);
	else if (node->type == Token::BitwiseOr || node->type == Token::LogicalOr)
		codeGen.emitOrRR(x64::RAX, x64::RCX);
	else if (node->type == Token::BitwiseXor)
		codeGen.emitXorRR(x64::RAX, x64::RCX);
	else if (node->type == Token::ShiftLeft)
		codeGen.emitShlRCl(x64::RAX);
	else if (node->type == Token::ShiftRight)
		codeGen.emitShrRCl(x64::RAX);

	else if (node->type == Token::Equal)
	{
		codeGen.emitMovRR(x64::RBX, x64::RAX);
		codeGen.emitXorRR(x64::RAX, x64::RAX);
		codeGen.emitCmpRR(x64::RBX, x64::RCX);
		codeGen.emitSetE(x64::RAX);
	}
	else if (node->type == Token::NotEqual)
	{
		codeGen.emitMovRR(x64::RBX, x64::RAX);
		codeGen.emitXorRR(x64::RAX, x64::RAX);
		codeGen.emitCmpRR(x64::RBX, x64::RCX);
		codeGen.emitSetNe(x64::RAX);
	}
	else if (node->type == Token::LessThan)
	{
		codeGen.emitMovRR(x64::RBX, x64::RAX);
		codeGen.emitXorRR(x64::RAX, x64::RAX);
		codeGen.emitCmpRR(x64::RBX, x64::RCX);
		codeGen.emitSetL(x64::RAX);
	}
	else if (node->type == Token::GreaterThan)
	{
		codeGen.emitMovRR(x64::RBX, x64::RAX);
		codeGen.emitXorRR(x64::RAX, x64::RAX);
		codeGen.emitCmpRR(x64::RBX, x64::RCX);
		codeGen.emitSetG(x64::RAX);
	}
	else if (node->type == Token::LessOrEqual)
	{
		codeGen.emitMovRR(x64::RBX, x64::RAX);
		codeGen.emitXorRR(x64::RAX, x64::RAX);
		codeGen.emitCmpRR(x64::RBX, x64::RCX);
		codeGen.emitSetLe(x64::RAX);
	}
	else if (node->type == Token::GreaterOrEqual)
	{
		codeGen.emitMovRR(x64::RBX, x64::RAX);
		codeGen.emitXorRR(x64::RAX, x64::RAX);
		codeGen.emitCmpRR(x64::RBX, x64::RCX);
		codeGen.emitSetGe(x64::RAX);
	}

	codeGen.emitPush(x64::RAX);
}

void CodeGenPass::visit(CallExpression* node)
{
	codeGen.emitSubRIm8(x64::RSP, 32);

	for (auto it = node->args.rbegin(), end = node->args.rend(); it != end; ++it)
	{
		AstVisitor::visit((*it).get());
	}

	if (node->args.size() >= 1)
		codeGen.emitPop(x64::RCX);
	if (node->args.size() >= 2)
		codeGen.emitPop(x64::RDX);
	if (node->args.size() >= 3)
		codeGen.emitPop(x64::R8);
	if (node->args.size() >= 4)
		codeGen.emitPop(x64::R9);

	codeGen.emitCallRipRel32(ctx.getSymbol(node->functionIdentifier));
	codeGen.emitSubRIm8(x64::RSP, 32);
	codeGen.emitPush(x64::RAX);
}

void CodeGenPass::visit(IndexExpression* node)
{
	
}

size_t CodeGenPass::align(size_t value, size_t alignment)
{
	return ceil(value / (double)alignment) * alignment;
}