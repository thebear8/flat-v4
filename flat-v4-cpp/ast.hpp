#pragma once
#include <memory>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "token.hpp"
#include "type.hpp"
#include "visitor.hpp"

namespace AstExp
{
	using Visitor = visitor::Visitor<
		struct AstNode,
		struct Declaration,
		struct Statement,
		struct Expression,
		struct UnaryExpression,
		struct BinaryExpression,
		struct IntegerExpression,
		struct IdentifierExpression,
		struct CallExpression,
		struct IndexExpression,
		struct BlockStatement,
		struct VariableStatement,
		struct ReturnStatement,
		struct WhileStatement,
		struct IfStatement,
		struct FunctionDeclaration,
		struct Module
	>;

	template<typename T>
	using Visitable = ::Visitable<T,
		struct AstNode,
		struct Declaration,
		struct Statement,
		struct Expression,
		struct UnaryExpression,
		struct BinaryExpression,
		struct IntegerExpression,
		struct IdentifierExpression,
		struct CallExpression,
		struct IndexExpression,
		struct BlockStatement,
		struct VariableStatement,
		struct ReturnStatement,
		struct WhileStatement,
		struct IfStatement,
		struct FunctionDeclaration,
		struct Module
	>;

	struct AstNode : public Visitable<AstNode>
	{
		size_t begin, end;

		AstNode(size_t begin, size_t end) :
			begin(begin), end(end) { }
	};

	struct Declaration : public AstNode, public Visitable<Declaration>
	{
		Declaration(size_t begin, size_t end) :
			AstNode(begin, end) { }
	};

	struct Statement : public AstNode
	{
		Statement(size_t begin, size_t end) :
			AstNode(begin, end) { }
	};

	struct Expression : public Statement
	{
		Expression(size_t begin, size_t end) :
			Statement(begin, end) { }
	};

	///////////////////////////////////////////

	struct UnaryExpression : public Expression
	{
		Token type;
		std::shared_ptr<Expression> expression;

		UnaryExpression(size_t begin, size_t end, Token type, std::shared_ptr<Expression> expression) :
			Expression(begin, end), type(type), expression(expression) { }
	};

	struct BinaryExpression : public Expression
	{
		Token type;
		std::shared_ptr<Expression> left, right;

		BinaryExpression(size_t begin, size_t end, Token type, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) :
			Expression(begin, end), type(type), left(left), right(right) { }
	};

	struct IntegerExpression : public Expression
	{
		std::string value;

		IntegerExpression(size_t begin, size_t end, std::string value) :
			Expression(begin, end), value(value) { }
	};

	struct IdentifierExpression : public Expression
	{
		std::string value;

		IdentifierExpression(size_t begin, size_t end, std::string value) :
			Expression(begin, end), value(value) { }
	};

	struct CallExpression : public Expression
	{
		std::shared_ptr<Expression> expression;
		std::vector<std::shared_ptr<Expression>> args;
		std::string functionIdentifier;

		CallExpression(size_t begin, size_t end, std::shared_ptr<Expression> expression, std::vector<std::shared_ptr<Expression>> args) :
			Expression(begin, end), expression(expression), args(args) { }
	};

	struct IndexExpression : public Expression
	{
		std::shared_ptr<Expression> expression;
		std::vector<std::shared_ptr<Expression>> args;

		IndexExpression(size_t begin, size_t end, std::shared_ptr<Expression> expression, std::vector<std::shared_ptr<Expression>> args) :
			Expression(begin, end), expression(expression), args(args) { }
	};

	///////////////////////////////////////////

	struct BlockStatement : public Statement
	{
		std::vector<std::shared_ptr<Statement>> statements;

		BlockStatement(size_t begin, size_t end, std::vector<std::shared_ptr<Statement>> statements) :
			Statement(begin, end), statements(statements) { }
	};

	struct VariableStatement : public Statement
	{
		std::vector<std::string> names;
		std::vector<std::shared_ptr<Expression>> values;

		VariableStatement(size_t begin, size_t end, std::vector<std::string> names, std::vector<std::shared_ptr<Expression>> values) :
			Statement(begin, end), names(names), values(values) { }
	};

	struct ReturnStatement : public Statement
	{
		std::shared_ptr<Expression> expression;

		ReturnStatement(size_t begin, size_t end, std::shared_ptr<Expression> expression) :
			Statement(begin, end), expression(expression) { }
	};

	struct WhileStatement : public Statement
	{
		std::shared_ptr<Expression> condition;
		std::shared_ptr<Statement> body;

		WhileStatement(size_t begin, size_t end, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> body) :
			Statement(begin, end), condition(condition), body(body) { }
	};

	struct IfStatement : public Statement
	{
		std::shared_ptr<Expression> condition;
		std::shared_ptr<Statement> ifBody, elseBody;

		IfStatement(size_t begin, size_t end, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> ifBody, std::shared_ptr<Statement> elseBody) :
			Statement(begin, end), condition(condition), ifBody(ifBody), elseBody(elseBody) { }
	};

	///////////////////////////////////////////

	struct FunctionDeclaration : public Declaration
	{
		std::string name;
		Type* result;
		std::vector<std::pair<std::string, Type*>> parameters;
		std::shared_ptr<Statement> body;
		std::vector<std::pair<std::string, Type*>> localVariables;

		FunctionDeclaration(size_t begin, size_t end, std::string name, Type* result, std::vector<std::pair<std::string, Type*>> parameters, std::shared_ptr<Statement> body) :
			Declaration(begin, end), name(name), result(result), parameters(parameters), body(body) { }
	};

	///////////////////////////////////////////

	struct Module : public AstNode
	{
		std::vector<std::shared_ptr<Declaration>> declarations;

		Module(size_t begin, size_t end, std::vector<std::shared_ptr<Declaration>> declarations) :
			AstNode(begin, end), declarations(declarations) { }
	};
}

using Visitor = visitor::Visitor<
	struct AstNode,
	struct IntegerExpression,
	struct IdentifierExpression,
	struct UnaryExpression,
	struct BinaryExpression,
	struct CallExpression,
	struct IndexExpression,
	struct BlockStatement,
	struct VariableStatement,
	struct ReturnStatement,
	struct WhileStatement,
	struct IfStatement,
	struct FunctionDeclaration,
	struct Module
>;

struct AstNode
{
	size_t begin, end;

	AstNode(size_t begin, size_t end) :
		begin(begin), end(end) { }

	virtual void accept(Visitor* visitor) { }
};

#define IMPLEMENT_ACCEPT() void accept(Visitor* visitor) override { visitor->visit(this); }

struct Declaration : public AstNode
{
	Declaration(size_t begin, size_t end) : 
		AstNode(begin, end) { }

	IMPLEMENT_ACCEPT()
};

struct Statement : public AstNode
{
	Statement(size_t begin, size_t end) : 
		AstNode(begin, end) { }

	IMPLEMENT_ACCEPT()
};

struct Expression : public Statement
{
	Expression(size_t begin, size_t end) : 
		Statement(begin, end) { }

	IMPLEMENT_ACCEPT()
};

///////////////////////////////////////////

struct UnaryExpression : public Expression
{
	Token type;
	std::shared_ptr<Expression> expression;

	UnaryExpression(size_t begin, size_t end, Token type, std::shared_ptr<Expression> expression) : 
		Expression(begin, end), type(type), expression(expression) { }

	IMPLEMENT_ACCEPT()
};

struct BinaryExpression : public Expression
{
	Token type;
	std::shared_ptr<Expression> left, right;

	BinaryExpression(size_t begin, size_t end, Token type, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) : 
		Expression(begin, end), type(type), left(left), right(right) { }

	IMPLEMENT_ACCEPT()
};

struct IntegerExpression : public Expression
{
	std::string value;

	IntegerExpression(size_t begin, size_t end, std::string value) : 
		Expression(begin, end), value(value) { }

	IMPLEMENT_ACCEPT()
};

struct IdentifierExpression : public Expression
{
	std::string value;

	IdentifierExpression(size_t begin, size_t end, std::string value) : 
		Expression(begin, end), value(value) { }

	IMPLEMENT_ACCEPT()
};

struct CallExpression : public Expression
{
	std::shared_ptr<Expression> expression;
	std::vector<std::shared_ptr<Expression>> args;
	std::string functionIdentifier;

	CallExpression(size_t begin, size_t end, std::shared_ptr<Expression> expression, std::vector<std::shared_ptr<Expression>> args) : 
		Expression(begin, end), expression(expression), args(args) { }

	IMPLEMENT_ACCEPT()
};

struct IndexExpression : public Expression
{
	std::shared_ptr<Expression> expression;
	std::vector<std::shared_ptr<Expression>> args;

	IndexExpression(size_t begin, size_t end, std::shared_ptr<Expression> expression, std::vector<std::shared_ptr<Expression>> args) : 
		Expression(begin, end), expression(expression), args(args) { }

	IMPLEMENT_ACCEPT()
};

///////////////////////////////////////////

struct BlockStatement : public Statement
{
	std::vector<std::shared_ptr<Statement>> statements;

	BlockStatement(size_t begin, size_t end, std::vector<std::shared_ptr<Statement>> statements) : 
		Statement(begin, end), statements(statements) { }

	IMPLEMENT_ACCEPT()
};

struct VariableStatement : public Statement
{
	std::vector<std::string> names;
	std::vector<std::shared_ptr<Expression>> values;

	VariableStatement(size_t begin, size_t end, std::vector<std::string> names, std::vector<std::shared_ptr<Expression>> values) : 
		Statement(begin, end), names(names), values(values) { }

	IMPLEMENT_ACCEPT()
};

struct ReturnStatement : public Statement
{
	std::shared_ptr<Expression> expression;

	ReturnStatement(size_t begin, size_t end, std::shared_ptr<Expression> expression) : 
		Statement(begin, end), expression(expression) { }

	IMPLEMENT_ACCEPT()
};

struct WhileStatement : public Statement
{
	std::shared_ptr<Expression> condition;
	std::shared_ptr<Statement> body;

	WhileStatement(size_t begin, size_t end, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> body) : 
		Statement(begin, end), condition(condition), body(body) { }

	IMPLEMENT_ACCEPT()
};

struct IfStatement : public Statement
{
	std::shared_ptr<Expression> condition;
	std::shared_ptr<Statement> ifBody, elseBody;

	IfStatement(size_t begin, size_t end, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> ifBody, std::shared_ptr<Statement> elseBody) : 
		Statement(begin, end), condition(condition), ifBody(ifBody), elseBody(elseBody) { }

	IMPLEMENT_ACCEPT()
};

///////////////////////////////////////////

struct FunctionDeclaration : public Declaration
{
	std::string name;
	Type* result;
	std::vector<std::pair<std::string, Type*>> parameters;
	std::shared_ptr<Statement> body;
	std::vector<std::pair<std::string, Type*>> localVariables;

	FunctionDeclaration(size_t begin, size_t end, std::string name, Type* result, std::vector<std::pair<std::string, Type*>> parameters, std::shared_ptr<Statement> body) : 
		Declaration(begin, end), name(name), result(result), parameters(parameters), body(body) { }

	IMPLEMENT_ACCEPT()
};

///////////////////////////////////////////

struct Module : public AstNode
{
	std::vector<std::shared_ptr<Declaration>> declarations;

	Module(size_t begin, size_t end, std::vector<std::shared_ptr<Declaration>> declarations) : 
		AstNode(begin, end), declarations(declarations) { }

	IMPLEMENT_ACCEPT()
};