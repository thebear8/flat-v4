#pragma once
#include "ast.hpp"
#include "blob.hpp"
#include "linker.hpp"
#include "code_generator.hpp"
#include "pe_generator.hpp"

class CodeGenPass : AstVisitor
{
public:
	Linker& ctx;
	CodeGenerator& codeGen;
	TypeContext& typeCtx;
	std::ostream& logStream;

	std::unordered_map<std::string, std::pair<Type*, size_t>> localVariables;
	std::unordered_map<std::string, std::vector<FunctionDeclaration>> functions;

	size_t uid;

public:
	CodeGenPass(Linker& ctx, CodeGenerator& codeGen, TypeContext& typeCtx, std::ostream& logStream) :
		ctx(ctx),
		codeGen(codeGen),
		typeCtx(typeCtx),
		logStream(logStream),
		uid(0)
	{
	}

public:
	void generateCode(std::unordered_map<std::string, std::vector<FunctionDeclaration>> functions);
	size_t align(size_t value, size_t alignment);

public:
	void visit(IntegerExpression* node) override;
	void visit(IdentifierExpression* node) override;
	void visit(UnaryExpression* node) override;
	void visit(BinaryExpression* node) override;
	void visit(CallExpression* node) override;
	void visit(IndexExpression* node) override;

	void visit(BlockStatement* node) override;
	void visit(VariableStatement* node) override;
	void visit(ReturnStatement* node) override;
	void visit(WhileStatement* node) override;
	void visit(IfStatement* node) override;

	void visit(FunctionDeclaration* node) override;
	void visit(Module* node) override;
};