using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace flat_v4
{
    class AstNode
    {
    }

    class Program : AstNode 
    {
        public Function[]? Functions { get; set; }
    }

    class Function : AstNode
    {
        public string? Name { get; set; }
        public BaseType? Result { get; set; }
        public BaseType[]? Parameters { get; set; }
        public Statement[]? Statements { get; set; }
    }

    class Statement : AstNode { }

    class VariableStatement : Statement
    {
        public string? Name { get; set; }
        public Expression? Value { get; set; }
    }

    class ReturnStatement : Statement
    {
        public Expression? Value { get; set; }
    }

    class WhileStatement : Statement
    {
        public Expression? Predicate { get; set; }
        public Statement? Action { get; set; }
    }

    class IfStatement : Statement
    {
        public Expression? Predicate { get; set; }
        public Statement? IfAction { get; set; }
        public Statement? ElseAction { get; set; }
    }

    ///////////////////////////////

    class Expression : AstNode { }

    class LiteralExpression : Expression
    {
        public Object? Value { get; set; }
    }

    class IdentifierExpression : Expression
    {
        public String? Name { get; set; }
    }

    class UnaryExpression : Expression
    {
        public Token Type { get; set; }
        public Expression? Operand { get; set; }
    }

    class BinaryExpression : Expression
    {
        public Token Type { get; set; }
        public Expression? Left { get; set; }
        public Expression? Right { get; set; }
    }

    class CallExpression : Expression
    {
        public string? Name { get; set; }
        public Expression[]? Parameters { get; set; }
    }

    class IndexExpression : Expression
    {
        public string? Name { get; set; }
        public Expression[]? Parameters { get; set; }
    }

    ///////////////////////////////

    class BaseType : AstNode { }

    class NamedType : BaseType
    {
        public string? Name { get; set; }
    }

    class PointerType : BaseType
    {
        public BaseType? Type { get; set; }
    }

    class ArrayType : BaseType
    {
        public BaseType? Type { get; set; }
    }
}
