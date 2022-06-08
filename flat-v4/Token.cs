using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace flat_v4
{
    enum Token
    {
		Eof,
		Error,
		Whitespace,

		Comma,
		Colon,

		ParenOpen,
		ParenClose,
		BracketOpen,
		BracketClose,
		BraceOpen,
		BraceClose,

		Plus,
		Minus,
		LogicalNot,
		BitwiseNot,
		Multiply,
		Divide,
		Modulo,
		ShiftLeft,
		ShiftRight,
		BitwiseAnd,
		BitwiseOr,
		BitwiseXor,
		Equal,
		NotEqual,
		LessThan,
		GreaterThan,
		LessOrEqual,
		GreaterOrEqual,
		LogicalAnd,
		LogicalOr,
		Assign,

		Let,
		Return,
		While,
		If,
		Else,
		Function,

		Integer,
		Identifier,
	}
}
