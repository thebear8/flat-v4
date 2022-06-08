#include <iostream>
#include "third_party/cli11/cli11.hpp"

#include "parser.hpp"
#include "semantic_pass.hpp"

/*
struct AstDump
{
private:
	int indentLevel = 0;
	std::stringstream output;

public:
	AstDump(std::shared_ptr<AstNode> ast)
	{
		node(ast);
	}

public:
	auto text() { return output.str(); }

private:
	std::string indent()
	{
		return indent(indentLevel);
	}

	std::string indent(int level)
	{
		std::string output;
		for (int i = 0; i < level; i++) output += "  ";
		return output;
	}

	void item(std::string value)
	{
		output << indent() << value << "\n";
	}

	void item(std::shared_ptr<AstNode> ast, bool doIndent = false)
	{
		node(ast, doIndent);
	}

	void field(std::string name, std::string value)
	{
		output << indent() << name << " = " << value << "\n";
	}

	void field(std::string name, std::shared_ptr<AstNode> ast, bool doIndent = false)
	{
		output << indent() << name << " = ";
		node(ast, doIndent);
	}

	void node(std::shared_ptr<AstNode> ast, bool doIndent = false)
	{
		if (!ast) {
			output << indent(doIndent * indentLevel) << "null\n";
			return;
		}

		output << indent(doIndent * indentLevel) << std::string(typeid(*ast).name()).substr(strlen("struct ")) << ":\n";

		indentLevel++;
		if (ast->is<UnaryExpression>()) {
			field("type", tokenNames[(size_t)ast->as<UnaryExpression>()->type]);
			field("expression", ast->as<UnaryExpression>()->expression);
		}
		else if (ast->is<BinaryExpression>()) {
			field("type", tokenNames[(size_t)ast->as<BinaryExpression>()->type]);
			field("left", ast->as<BinaryExpression>()->left);
			field("right", ast->as<BinaryExpression>()->right);
		}
		else if (ast->is<IntegerExpression>()) {
			field("value", std::string(ast->as<IntegerExpression>()->value));
		}
		else if (ast->is<IdentifierExpression>()) {
			field("value", std::string(ast->as<IdentifierExpression>()->value));
		}
		else if (ast->is<CallExpression>()) {
			field("expression", ast->as<CallExpression>()->expression);
			field("args", "");
			indentLevel++;
			for (auto& arg : ast->as<CallExpression>()->args) {
				item(arg, true);
			}
			indentLevel--;
		}
		else if (ast->is<IndexExpression>()) {
			field("expression", ast->as<IndexExpression>()->expression);
			field("args", "");
			indentLevel++;
			for (auto& arg : ast->as<IndexExpression>()->args) {
				item(arg, true);
			}
			indentLevel--;
		}
		else if (ast->is<BlockStatement>()) {
			field("statements", "");
			indentLevel++;
			for (auto& statement : ast->as<BlockStatement>()->statements) {
				item(statement, true);
			}
			indentLevel--;
		}
		else if (ast->is<VariableStatement>()) {
			for (int i = 0; i < ast->as<VariableStatement>()->names.size(); i++) {
				field(std::string(ast->as<VariableStatement>()->names[i]), ast->as<VariableStatement>()->values[i]);
			}
		}
		else if (ast->is<ReturnStatement>()) {
			field("expression", ast->as<ReturnStatement>()->expression);
		}
		else if (ast->is<WhileStatement>()) {
			field("condition", ast->as<WhileStatement>()->condition);
			field("body", ast->as<WhileStatement>()->body);
		}
		else if (ast->is<IfStatement>()) {
			field("condition", ast->as<IfStatement>()->condition);
			field("ifBody", ast->as<IfStatement>()->ifBody);
			field("elseBody", ast->as<IfStatement>()->elseBody);
		}
		else if (ast->is<FunctionDeclaration>()) {
			field("name", std::string(ast->as<FunctionDeclaration>()->name));
			field("result", typeName(ast->as<FunctionDeclaration>()->result));
			field("parameters", "");
			indentLevel++;
			for (auto& p : ast->as<FunctionDeclaration>()->parameters) {
				field(std::string(p.first), typeName(p.second));
			}
			indentLevel--;
			field("body", ast->as<FunctionDeclaration>()->body);
		}
		indentLevel--;
	}

	std::string typeName(std::shared_ptr<Type> type)
	{
		if (dynamic_cast<ArrayType*>(type.get())) {
			auto t = dynamic_cast<ArrayType*>(type.get());
			return typeName(t->base) + "[]";
		} 
		else if (dynamic_cast<PointerType*>(type.get())) {
			auto t = dynamic_cast<PointerType*>(type.get());
			return typeName(t->base) + "*";
		} 
		else {
			auto t = dynamic_cast<NamedType*>(type.get());
			return std::string(t->name);
		}
	}
};
*/

int main(int argc, char* argv[])
{
	std::string inputFile, outputFile;

	CLI::App app("flc");
	app.add_option("input, -i, --input", inputFile)->required()->check(CLI::ExistingFile);
	app.add_option("output, -o, --output", outputFile)->required();

	CLI11_PARSE(app, argc, argv);

	std::ifstream in{ inputFile };
	std::string input{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

	TypeContext ctx(64);
	ctx.builtinTypes.try_emplace("u8", new BuiltinType(ctx, "u8", 8));
	ctx.builtinTypes.try_emplace("u16", new BuiltinType(ctx, "u16", 16));
	ctx.builtinTypes.try_emplace("u32", new BuiltinType(ctx, "u32", 32));
	ctx.builtinTypes.try_emplace("u64", new BuiltinType(ctx, "u64", 64));
	ctx.builtinTypes.try_emplace("i8", new BuiltinType(ctx, "i8", 8));
	ctx.builtinTypes.try_emplace("i16", new BuiltinType(ctx, "i16", 16));
	ctx.builtinTypes.try_emplace("i32", new BuiltinType(ctx, "i32", 32));
	ctx.builtinTypes.try_emplace("i64", new BuiltinType(ctx, "i64", 64));
	ctx.builtinTypes.try_emplace("bool", new BuiltinType(ctx, "bool", 1));
	ctx.builtinTypes.try_emplace("char", new BuiltinType(ctx, "char", 8));
	ctx.builtinTypes.try_emplace("pointer", new BuiltinType(ctx, "pointer", 64));

	std::string ast;
	Parser parser(ctx, input, std::cout);
	auto program = parser.module();
	auto pass = SemanticValidationPass(ctx, input, std::cout);
	pass.extractFunctions(program.get());
	pass.validateFunctions();

	/*for (auto& decl : program->declarations) {
		//ast += AstDump(decl).text();
	}



	auto functions = DeclarationPass(program.get()).functions;

	for (auto& function : functions)
	{
		SemanticPass(functions).analyse(function);
	}*/

	std::ofstream out{ outputFile };
	out << ast;
	out.close();
}