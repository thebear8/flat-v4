#pragma once
#include <string>
#include <string_view>
#include <iostream>

#include "token.hpp"

class Lexer
{
private:
	static constexpr std::pair<Token, std::string_view> tokens[] =
	{
		{ Token::Comma, "," },
		{ Token::Colon, ":" },

		{ Token::ParenOpen, "(" },
		{ Token::ParenClose, ")" },
		{ Token::BracketOpen, "[" },
		{ Token::BracketClose, "]" },
		{ Token::BraceOpen, "{" },
		{ Token::BraceClose, "}" },

		{ Token::Plus, "+" },
		{ Token::Minus, "-" },
		{ Token::LogicalNot, "!" },
		{ Token::BitwiseNot, "~" },
		{ Token::Multiply, "*" },
		{ Token::Divide, "/" },
		{ Token::Modulo, "%" },
		{ Token::ShiftLeft, "<<" },
		{ Token::ShiftRight, ">>" },
		{ Token::BitwiseAnd, "&" },
		{ Token::BitwiseOr, "|" },
		{ Token::BitwiseXor, "^" },
		{ Token::Equal, "==" },
		{ Token::NotEqual, "!=" },
		{ Token::LessThan, "<" },
		{ Token::GreaterThan, ">" },
		{ Token::LessOrEqual, "<=" },
		{ Token::GreaterOrEqual, ">=" },
		{ Token::LogicalAnd, "&&" },
		{ Token::LogicalOr, "||" },
		{ Token::Assign, "=" },
	};

	static constexpr std::pair<Token, std::string_view> keywords[] =
	{
		{ Token::Let, "let" },
		{ Token::Return, "return" },
		{ Token::While, "while" },
		{ Token::If, "if" },
		{ Token::Else, "else" },
		{ Token::Function, "fn" },
	};

protected:
	size_t position;
	std::string_view input;
	std::string_view value;
	std::ostream& logStream;

public:
	Lexer(std::string_view input, std::ostream& logStream) :
		position(0),
		input(input),
		value(""),
		logStream(logStream)
	{
	}

private:
	void trim()
	{
		while (position < input.length() && isWhitespace(input[position]))
			position++;
	}

	Token advance()
	{
		if (position >= input.length())
		{
			value = "";
			return Token::Eof;
		}

		size_t length = 0;
		Token type = (Token)(-1);
		for (auto& token : tokens)
		{
			if (length < token.second.length() && input.substr(position, token.second.length()) == token.second)
			{
				length = token.second.length();
				type = token.first;
			}
		}

		if (type != (Token)(-1))
		{
			value = input.substr(position, length);
			position += length;
			return type;
		}

		if (isDigit(input[position]))
		{
			size_t start = position;
			while (position < input.length() && isDigit(input[position])) 
				position++;
			value = input.substr(start, position - start);
			return Token::Integer;
		}

		if (isIdentifier(input[position]))
		{
			size_t start = position;
			while (position < input.length() && isIdentifier(input[position])) 
				position++;
			value = input.substr(start, position - start);

			for (auto& keyword : keywords)
			{
				if (input.substr(start, position - start) == keyword.second)
				{
					return keyword.first;
				}
			}

			return Token::Identifier;
		}

		error("Invalid Token", true);
		return Token::Error;
	}

public:
	bool match(Token expected)
	{
		trim();
		size_t start = position;
		Token actual = advance();
		if (actual == expected)
			return true;
		if (actual == Token::Eof)
			error("Unexpected EOF", true);
		position = start;
		return false;
	}

	bool expect(Token expected)
	{
		trim();
		size_t start = position;
		Token actual = advance();
		if (actual == expected)
			return true;
		position = start;
		error("Unexpected Token " + std::string(value) + ", expected " + tokenNames[(size_t)expected], true);
		return false;
	}

public:
	std::string integer()
	{
		return std::string(value);
	}

	std::string identifier()
	{
		return std::string(value);
	}

public:
	void error(std::string message, bool fatal = false)
	{
		size_t line = 1, column = 1;
		for (size_t i = 0; i < position && i < input.length(); i++) {
			if (input[i] == '\n') {
				line++;
				column = 1;
			} else {
				column++;
			}
		}

		logStream << "ln " << line << ", col " << column << ": " << message << "\n";
		if(fatal) throw std::exception(message.c_str());
	}

private:
	bool isDigit(char c) { return (c >= '0' && c <= '9'); }
	bool isLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
	bool isWhitespace(char c) { return (c == ' ' || c == '\t' || c == '\r' || c == '\n'); }
	bool isIdentifier(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_'); }
};