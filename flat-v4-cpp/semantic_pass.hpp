#pragma once
#include "ast.hpp"
#include <iostream>

class SemanticValidationPass : AstVisitor
{
public:
	TypeContext& typeCtx;
	std::string_view source;
	std::ostream& logStream;

	std::unordered_map<std::string, std::vector<FunctionDeclaration>> functions;

	Type* expressionResult;
	Type* functionResult;

	std::unordered_map<std::string, Type*> localVariables;

public:
	SemanticValidationPass(TypeContext& typeCtx, std::string_view source, std::ostream& logStream) :
		typeCtx(typeCtx),
		source(source),
		logStream(logStream),
		expressionResult(nullptr),
		functionResult(nullptr)
	{
	}

public:
	void extractFunctions(Module* node);
	void validateFunctions();

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

public:
	bool hasFunction(std::string const& name, std::vector<Type*> const& args);
	FunctionDeclaration const& getFunction(std::string const& name, std::vector<Type*> const& args);

public:
	void reportError(AstNode* node, std::string msg);

public:
	static const inline std::unordered_map<Token, std::string> unaryOperatorNames =
	{
		{ Token::Plus, "__positive__" },
		{ Token::Minus, "__negative__" },
		{ Token::LogicalNot, "__not__" },
		{ Token::BitwiseNot, "__bitnot__" },
	};

	static const inline std::unordered_map<Token, std::string> binaryOperatorNames =
	{
		{ Token::Plus, "__add__" },
		{ Token::Minus, "__subtract__" },
		{ Token::Multiply, "__multiply__" },
		{ Token::Divide, "__divide__" },
		{ Token::Modulo, "__modulo__" },

		{ Token::BitwiseAnd, "__bitand__" },
		{ Token::BitwiseNot, "__bitor__" },
		{ Token::BitwiseXor, "__bitxor__" },
		{ Token::ShiftLeft, "__lshift__" },
		{ Token::ShiftRight, "__rshift__" },

		{ Token::LogicalAnd, "__and__" },
		{ Token::LogicalOr, "__or__" },

		{ Token::Equal, "__equal__" },
		{ Token::NotEqual, "__notequal__" },
		{ Token::LessThan, "__less__" },
		{ Token::GreaterThan, "__greater__" },
		{ Token::LessOrEqual, "__lessorequal__" },
		{ Token::GreaterOrEqual, "__greaterorequal__" },
	};
};