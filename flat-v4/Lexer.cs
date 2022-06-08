using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace flat_v4
{
	internal class Lexer
	{
		private int position;
		private string input;
		private string[] groups;

		public Dictionary<Token, Regex> tokens = new()
		{
			{ Token.Whitespace, new Regex(@"\s+") },
			{ Token.Comma, new Regex(@",") },
			{ Token.Colon, new Regex(@":") },

			{ Token.ParenOpen, new Regex(@"\(") },
			{ Token.ParenClose, new Regex(@"\)") },
			{ Token.BracketOpen, new Regex(@"\[") },
			{ Token.BracketClose, new Regex(@"\]") },
			{ Token.BraceOpen, new Regex(@"\{") },
			{ Token.BraceClose, new Regex(@"\}") },

			{ Token.Plus, new Regex(@"\+") },
			{ Token.Minus, new Regex(@"\-") },
			{ Token.LogicalNot, new Regex(@"!") },
			{ Token.BitwiseNot, new Regex(@"~") },
			{ Token.Multiply, new Regex(@"\*") },
			{ Token.Divide, new Regex(@"/") },
			{ Token.Modulo, new Regex(@"%") },
			{ Token.ShiftLeft, new Regex(@"<<") },
			{ Token.ShiftRight, new Regex(@">>") },
			{ Token.BitwiseAnd, new Regex(@"&") },
			{ Token.BitwiseOr, new Regex(@"\|") },
			{ Token.BitwiseXor, new Regex(@"\^") },
			{ Token.Equal, new Regex(@"==") },
			{ Token.NotEqual, new Regex(@"!=") },
			{ Token.LessThan, new Regex(@"<") },
			{ Token.GreaterThan, new Regex(@">") },
			{ Token.LessOrEqual, new Regex(@"<=") },
			{ Token.GreaterOrEqual, new Regex(@">=") },
			{ Token.LogicalAnd, new Regex(@"&&") },
			{ Token.LogicalOr, new Regex(@"\|\|") },
			{ Token.Assign, new Regex(@"=") },

			{ Token.Let, new Regex(@"let") },
			{ Token.Return, new Regex(@"return") },
			{ Token.While, new Regex(@"while") },
			{ Token.If, new Regex(@"if") },
			{ Token.Else, new Regex(@"else") },
			{ Token.Function, new Regex(@"fn") },

			{ Token.Integer, new Regex(@"[0-9]+") },
			{ Token.Identifier, new Regex(@"[a-zA-Z_][a-zA-Z0-9_]*") },
		};

		public Lexer(string input)
		{
			this.position = 0;
			this.input = input;
			this.groups = new string[1];
		}

		public Token next()
		{
			while (position < input.Length && char.IsWhiteSpace(input[position]))
				position++;

			if (position >= input.Length - 1)
				return Token.Eof;

			int length = 0;
			Token type = Token.Error;
			foreach(var token in tokens)
            {
				var match = token.Value.Match(input, position);
				if(match.Success && match.Length > length)
                {
					length = match.Length;
					type = token.Key;
					groups = match.Groups.Values.Select((g) => g.Value).ToArray();
                }
            }

			return type;
		}

		public bool match(Token expected)
        {
			var state = position;
			var actual = next();
			if (actual == expected)
				return true;
			if (actual == Token.Eof)
				throw new Exception("Unexpected EOF");
			position = state;
			return false;
        }

		public void expect(Token expected)
        {
			var state = position;
			var actual = next();
			if (actual == expected)
				return;
			position = state;
			error($"Unexpected {actual}, expected {expected}");
        }

		public void error(string message)
        {
			int line = 1, column = 1;
			for (int i = 0; i < input.Length && i < position; i++)
			{
				if (input[i] == '\n')
				{
					line++;
					column = 0;
				}
				column++;
			}

			throw new Exception($"Line {line}, column {column}: {message}");
		}

		public string getValue(int group)
        {
			return groups[group];
        }
	}
}
