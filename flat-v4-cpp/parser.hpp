#pragma once
#include "lexer.hpp"
#include "ast.hpp"
#include "type.hpp"

class Parser : public Lexer
{
private:
	TypeContext& ctx;

public:
	Parser(TypeContext& ctx, std::string_view input, std::ostream& logStream) : 
		Lexer(input, logStream),
		ctx(ctx)
	{
	}

public:
	std::shared_ptr<Expression> l0()
	{
		auto begin = position;
		if (match(Token::ParenOpen)) {
			auto e = expression();
			if (!expect(Token::ParenClose)) return nullptr;
			return e;
		} else if(match(Token::Integer)) {
			return std::make_shared<IntegerExpression>(begin, position, integer());
		} else if(match(Token::Identifier)) {
			return std::make_shared<IdentifierExpression>(begin, position, identifier());
		} else {
			error("Invalid l0", true);
			return nullptr;
		}
	}

	std::shared_ptr<Expression> l1()
	{
		auto begin = position;
		auto e = l0();
		while(true) {
			if(match(Token::ParenOpen)) {
				std::vector<std::shared_ptr<Expression>> args;
				while(!match(Token::ParenClose) && !match(Token::Eof)) {
					args.push_back(expression());
					match(Token::Comma);
				}
				e = std::make_shared<CallExpression>(begin, position, e, args);
			} else if(match(Token::BracketOpen)) {
				std::vector<std::shared_ptr<Expression>> args;
				while(!match(Token::BracketClose) && !match(Token::Eof)) {
					args.push_back(expression());
					match(Token::Comma);
				}
				e = std::make_shared<IndexExpression>(begin, position, e, args);
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l2()
	{
		auto begin = position;
		if(match(Token::Plus)) {
			return std::make_shared<UnaryExpression>(begin, position, Token::Plus, l2());
		} else if(match(Token::Minus)) {
			return std::make_shared<UnaryExpression>(begin, position, Token::Minus, l2());
		} else if(match(Token::LogicalNot)) {
			return std::make_shared<UnaryExpression>(begin, position, Token::LogicalNot, l2());
		} else if(match(Token::BitwiseNot)) {
			return std::make_shared<UnaryExpression>(begin, position, Token::BitwiseNot, l2());
		} else {
			return l1();
		}
	}

	std::shared_ptr<Expression> l3()
	{
		auto begin = position;
		auto e = l2();
		while(true) {
			if(match(Token::Multiply)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::Multiply, e, l2());
			} else if(match(Token::Divide)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::Divide, e, l2());
			} else if(match(Token::Modulo)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::Modulo, e, l2());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l4()
	{
		auto begin = position;
		auto e = l3();
		while(true) {
			if(match(Token::Plus)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::Plus, e, l3());
			} else if(match(Token::Minus)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::Minus, e, l3());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l5()
	{
		auto begin = position;
		auto e = l4();
		while(true) {
			if(match(Token::ShiftLeft)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::ShiftLeft, e, l4());
			} else if(match(Token::ShiftRight)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::ShiftRight, e, l4());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l6()
	{
		auto begin = position;
		auto e = l5();
		while(true) {
			if(match(Token::BitwiseAnd)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::BitwiseAnd, e, l5());
			} else if(match(Token::BitwiseOr)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::BitwiseOr, e, l5());
			} else if(match(Token::BitwiseXor)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::BitwiseXor, e, l5());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l7()
	{
		auto begin = position;
		auto e = l6();
		while(true) {
			if(match(Token::Equal)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::Equal, e, l6());
			} else if(match(Token::NotEqual)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::NotEqual, e, l6());
			} else if(match(Token::LessThan)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::LessThan, e, l6());
			} else if(match(Token::GreaterThan)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::GreaterThan, e, l6());
			} else if(match(Token::LessOrEqual)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::LessOrEqual, e, l6());
			} else if(match(Token::GreaterOrEqual)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::GreaterOrEqual, e, l6());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l8()
	{
		auto begin = position;
		auto e = l7();
		while(true) {
			if(match(Token::LogicalAnd)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::LogicalAnd, e, l7());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l9()
	{
		auto begin = position;
		auto e = l8();
		while(true) {
			if(match(Token::LogicalOr)) {
				e = std::make_shared<BinaryExpression>(begin, position, Token::LogicalOr, e, l8());
			} else {
				return e;
			}
		}
	}

	std::shared_ptr<Expression> l10()
	{
		auto begin = position;
		auto e = l9();
		if(match(Token::Assign)) {
			return std::make_shared<BinaryExpression>(begin, position, Token::Assign, e, l10());
		} else {
			return e;
		}
	}

	std::shared_ptr<Expression> expression()
	{
		return l10();
	}

	std::shared_ptr<Statement> blockStatement()
	{
		auto begin = position;
		std::vector<std::shared_ptr<Statement>> statements;
		while(!match(Token::BraceClose) && !match(Token::Eof)) {
			statements.push_back(statement());
		}
		return std::make_shared<BlockStatement>(begin, position, statements);
	}

	std::shared_ptr<Statement> variableStatement()
	{
		auto begin = position;
		std::vector<std::string> names;
		std::vector<std::shared_ptr<Expression>> values;
		while(match(Token::Identifier)) {
			names.push_back(identifier());
			if (!expect(Token::Assign)) return nullptr;
			values.push_back(expression());
			match(Token::Comma);
		}
		return std::make_shared<VariableStatement>(begin, position, names, values);
	}

	std::shared_ptr<Statement> returnStatement()
	{
		auto begin = position;
		return std::make_shared<ReturnStatement>(begin, position, expression());
	}

	std::shared_ptr<Statement> whileStatement()
	{
		auto begin = position;
		if (!expect(Token::ParenOpen)) return nullptr;
		auto condition = expression();
		if (!expect(Token::ParenClose)) return nullptr;
		auto body = statement();
		return std::make_shared<WhileStatement>(begin, position, condition, body);
	}

	std::shared_ptr<Statement> ifStatement()
	{
		auto begin = position;
		if (!expect(Token::ParenOpen)) return nullptr;
		auto condition = expression();
		if (!expect(Token::ParenClose)) return nullptr;
		auto ifBody = statement();
		if(match(Token::Else)) {
			return std::make_shared<IfStatement>(begin, position, condition, ifBody, statement());
		} else {
			return std::make_shared<IfStatement>(begin, position, condition, ifBody, nullptr);
		}
	}

	std::shared_ptr<Statement> statement()
	{
		if(match(Token::BraceOpen)) {
			return blockStatement();
		} else if(match(Token::Let)) {
			return variableStatement();
		} else if(match(Token::Return)) {
			return returnStatement();
		} else if(match(Token::While)) {
			return whileStatement();
		} else if(match(Token::If)) {
			return ifStatement();
		} else {
			return expression();
		}
	}

	std::shared_ptr<Declaration> functionDeclaration()
	{
		auto begin = position;
		if (!expect(Token::Identifier)) return nullptr;
		auto name = identifier();
		std::vector<std::pair<std::string, Type*>> parameters;
		if (!expect(Token::ParenOpen)) return nullptr;
		while(!match(Token::ParenClose) && !match(Token::Eof)) {
			if (!expect(Token::Identifier)) return nullptr;
			auto name = identifier();
			if (!expect(Token::Colon)) return nullptr;
			auto type = typeName();
			parameters.push_back({ name, type });
			match(Token::Comma);
		}
		Type* result = ctx.getNamedType("void");
		if(match(Token::Colon)) {
			result = typeName();
		}
		if (!expect(Token::BraceOpen)) return nullptr;
		auto body = blockStatement();
		return std::make_shared<FunctionDeclaration>(begin, position, name, result, parameters, body);
	}

	std::shared_ptr<Module> module()
	{
		auto begin = position;
		std::vector<std::shared_ptr<Declaration>> declarations;
		while(!match(Token::Eof)) {
			if (!expect(Token::Function)) return nullptr;
			declarations.push_back(functionDeclaration());
		}
		return std::make_shared<Module>(begin, position, declarations);
	}

	Type* typeName()
	{
		if (!expect(Token::Identifier)) return nullptr;
		Type* type = ctx.getNamedType(std::string(identifier()));
		while(true) {
			if(match(Token::Multiply)) {
				type = ctx.getPointerType(type);
			} else if(match(Token::BracketOpen) && match(Token::BracketClose)) {
				type = ctx.getArrayType(type);
			} else {
				return type;
			}
		}
	}
};