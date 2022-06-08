using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace flat_v4
{
	internal abstract class Parser : Lexer
	{
		public Parser(string input) : base(input)
		{
		}

		AstNode[] ArgumentList()
        {
			List<AstNode> args = new List<AstNode>();
			while(!match(Token.ParenClose))
            {
				args.Add(expression());
				match(Token.Comma);
            }
			return args.ToArray();
        }

		AstNode l0()
		{
			if(match(Token.ParenOpen))
			{
				var e = expression();
				expect(Token.ParenClose);
				return e;
			}
			else if(match(Token.Integer))
				return new LiteralExpression() { Value = Convert.ToInt64(getValue(0)) };
			else if(match(Token.Identifier))
				return new IdentifierExpression() { Name = getValue(0) };
			else
				throw new Exception("Invalid L0");
		}

		AstNode l1()
        {
			var e = l0();
			while(true)
            {
				if(match(Token.ParenOpen))
                {

                }
            }
        }


		public string identifier()
		{
			expect(Token.Identifier);
			return getValue(0);
		}

		public long integer()
		{
			expect(Token.Integer);
			return Convert.ToInt64(getValue(0));
		}

		public BaseType type()
		{
			BaseType type = new NamedType() { Name = Identifier() };
			while (true)
			{
				if (match(Token.Multiply))
					type = new PointerType() { Type = type };
				else if (match(Token.BracketOpen) && match(Token.BracketClose))
					type = new ArrayType() { Type = type };
				else
					return type;
			}
		}
	}
}
