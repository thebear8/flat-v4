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
	using Visitor = ::Visitor<
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

struct AstNode;
struct IntegerExpression;
struct IdentifierExpression;
struct UnaryExpression;
struct BinaryExpression;
struct CallExpression;
struct IndexExpression;

struct BlockStatement;
struct VariableStatement;
struct ReturnStatement;
struct WhileStatement;
struct IfStatement;

struct FunctionDeclaration;
struct Module;

struct AstVisitor
{
	virtual void visit(IntegerExpression* node) { }
	virtual void visit(IdentifierExpression* node) { }
	virtual void visit(UnaryExpression* node) { }
	virtual void visit(BinaryExpression* node) { }
	virtual void visit(CallExpression* node) { }
	virtual void visit(IndexExpression* node) { }

	virtual void visit(BlockStatement* node) { }
	virtual void visit(VariableStatement* node) { }
	virtual void visit(ReturnStatement* node) { }
	virtual void visit(WhileStatement* node) { }
	virtual void visit(IfStatement* node) { }

	virtual void visit(FunctionDeclaration* node) { }
	virtual void visit(Module* node) { }

	virtual void visit(AstNode* node);
};

struct AstNode
{
	size_t begin, end;

	AstNode(size_t begin, size_t end) :
		begin(begin), end(end) { }

	virtual void accept(AstVisitor* visitor) { }
};

struct Declaration : public AstNode
{
	Declaration(size_t begin, size_t end) : 
		AstNode(begin, end) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct Statement : public AstNode
{
	Statement(size_t begin, size_t end) : 
		AstNode(begin, end) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct Expression : public Statement
{
	Expression(size_t begin, size_t end) : 
		Statement(begin, end) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

///////////////////////////////////////////

struct UnaryExpression : public Expression
{
	Token type;
	std::shared_ptr<Expression> expression;

	UnaryExpression(size_t begin, size_t end, Token type, std::shared_ptr<Expression> expression) : 
		Expression(begin, end), type(type), expression(expression) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct BinaryExpression : public Expression
{
	Token type;
	std::shared_ptr<Expression> left, right;

	BinaryExpression(size_t begin, size_t end, Token type, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) : 
		Expression(begin, end), type(type), left(left), right(right) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct IntegerExpression : public Expression
{
	std::string value;

	IntegerExpression(size_t begin, size_t end, std::string value) : 
		Expression(begin, end), value(value) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct IdentifierExpression : public Expression
{
	std::string value;

	IdentifierExpression(size_t begin, size_t end, std::string value) : 
		Expression(begin, end), value(value) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct CallExpression : public Expression
{
	std::shared_ptr<Expression> expression;
	std::vector<std::shared_ptr<Expression>> args;
	std::string functionIdentifier;

	CallExpression(size_t begin, size_t end, std::shared_ptr<Expression> expression, std::vector<std::shared_ptr<Expression>> args) : 
		Expression(begin, end), expression(expression), args(args) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct IndexExpression : public Expression
{
	std::shared_ptr<Expression> expression;
	std::vector<std::shared_ptr<Expression>> args;

	IndexExpression(size_t begin, size_t end, std::shared_ptr<Expression> expression, std::vector<std::shared_ptr<Expression>> args) : 
		Expression(begin, end), expression(expression), args(args) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

///////////////////////////////////////////

struct BlockStatement : public Statement
{
	std::vector<std::shared_ptr<Statement>> statements;

	BlockStatement(size_t begin, size_t end, std::vector<std::shared_ptr<Statement>> statements) : 
		Statement(begin, end), statements(statements) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct VariableStatement : public Statement
{
	std::vector<std::string> names;
	std::vector<std::shared_ptr<Expression>> values;

	VariableStatement(size_t begin, size_t end, std::vector<std::string> names, std::vector<std::shared_ptr<Expression>> values) : 
		Statement(begin, end), names(names), values(values) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct ReturnStatement : public Statement
{
	std::shared_ptr<Expression> expression;

	ReturnStatement(size_t begin, size_t end, std::shared_ptr<Expression> expression) : 
		Statement(begin, end), expression(expression) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct WhileStatement : public Statement
{
	std::shared_ptr<Expression> condition;
	std::shared_ptr<Statement> body;

	WhileStatement(size_t begin, size_t end, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> body) : 
		Statement(begin, end), condition(condition), body(body) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

struct IfStatement : public Statement
{
	std::shared_ptr<Expression> condition;
	std::shared_ptr<Statement> ifBody, elseBody;

	IfStatement(size_t begin, size_t end, std::shared_ptr<Expression> condition, std::shared_ptr<Statement> ifBody, std::shared_ptr<Statement> elseBody) : 
		Statement(begin, end), condition(condition), ifBody(ifBody), elseBody(elseBody) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
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

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};

///////////////////////////////////////////

struct Module : public AstNode
{
	std::vector<std::shared_ptr<Declaration>> declarations;

	Module(size_t begin, size_t end, std::vector<std::shared_ptr<Declaration>> declarations) : 
		AstNode(begin, end), declarations(declarations) { }

	void accept(AstVisitor* visitor) override { visitor->visit(this); }
};