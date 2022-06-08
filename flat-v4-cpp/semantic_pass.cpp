#include "semantic_pass.hpp"
#include <iostream>

void SemanticValidationPass::extractFunctions(Module* node)
{
	visit(node);
}

void SemanticValidationPass::validateFunctions()
{
	for (auto& cluster : functions)
	{
		for (auto& function : cluster.second)
		{
			functionResult = function.result;

			// make parameters locally accessible
			localVariables.clear();
			for (auto& param : function.parameters)
			{
				localVariables.try_emplace(param.first, param.second);
			}

			AstVisitor::visit(function.body.get());
		}
	}
}

void SemanticValidationPass::visit(IntegerExpression* node)
{
	expressionResult = typeCtx.getNamedType("i64");
}

void SemanticValidationPass::visit(IdentifierExpression* node)
{
	if (!localVariables.contains(node->value))
		reportError(node, "Undefined Identifier");
	expressionResult = localVariables.at(node->value);
}

void SemanticValidationPass::visit(UnaryExpression* node)
{
	std::vector<Type*> args;
	AstVisitor::visit(node->expression.get());
	args.push_back(expressionResult);

	if (!unaryOperatorNames.contains(node->type))
		reportError(node, "Unknown unary operator");

	if (!hasFunction(unaryOperatorNames.at(node->type), args))
		reportError(node, "No matching operator function found");

	expressionResult = getFunction(unaryOperatorNames.at(node->type), args).result;
}

void SemanticValidationPass::visit(BinaryExpression* node)
{
	if (node->type == Token::Assign)
	{
		AstVisitor::visit(node->left.get());
		auto left = expressionResult;
		AstVisitor::visit(node->right.get());
		auto right = expressionResult;

		if (!Type::areSame(left, right))
			reportError(node, "Assignment type mismatch");

		expressionResult = left;
	}
	else
	{
		std::vector<Type*> args;
		AstVisitor::visit(node->left.get());
		args.push_back(expressionResult);
		AstVisitor::visit(node->right.get());
		args.push_back(expressionResult);

		if (!binaryOperatorNames.contains(node->type))
			reportError(node, "Unknown binary operator");

		if (!hasFunction(binaryOperatorNames.at(node->type), args))
			reportError(node, "No matching operator function found");

		expressionResult = getFunction(binaryOperatorNames.at(node->type), args).result;
	}
}

void SemanticValidationPass::visit(CallExpression* node)
{
	std::vector<Type*> args;
	for (auto& arg : node->args)
	{
		AstVisitor::visit(arg.get());
		args.push_back(expressionResult);
	}

	std::string name = "";
	if (dynamic_cast<IdentifierExpression*>(node->expression.get()))
	{
		name = dynamic_cast<IdentifierExpression*>(node->expression.get())->value;

		node->functionIdentifier = name + "(";
		for (size_t i = 0; i < args.size(); i++)
		{
			node->functionIdentifier += args[i]->toString();
			if (i != args.size())
				node->functionIdentifier += ",";
		}
		node->functionIdentifier += ")";
	}
	else
	{
		// NOT IMPLEMENTED FULLY YET
		throw std::exception("not implemented");

		name = "__call__";
		AstVisitor::visit(node->expression.get());
		args.insert(args.begin(), expressionResult);
	}

	if (!hasFunction(name, args))
		reportError(node, "No matching function was found");

	expressionResult = getFunction(name, args).result;
}

void SemanticValidationPass::visit(IndexExpression* node)
{
	AstVisitor::visit(node->expression.get());
	auto valueType = expressionResult;

	if (dynamic_cast<ArrayType*>(valueType))
	{
		if (node->args.size() != 1)
			reportError(node, "Invalid parameter count for basic index expression");

		AstVisitor::visit(node->args[0].get());
		auto indexType = expressionResult;
		if (!Type::areSame(indexType, typeCtx.getNamedType("i64")))
			reportError(node, "Invalid index type");

		expressionResult = dynamic_cast<ArrayType*>(valueType)->base;
	}
	else if (dynamic_cast<NamedType*>(valueType))
	{
		// NOT IMPLEMENTED FULLY YET
		throw std::exception("not implemented");

		std::vector<Type*> args;
		for (auto& arg : node->args)
		{
			AstVisitor::visit(arg.get());
			args.push_back(expressionResult);
		}

		if (!hasFunction("__index__", args))
			reportError(node, "No matching index operator function found");

		expressionResult = getFunction("__index__", args).result;
	}
	else
	{
		reportError(node, "Invalid value type for index expression");
	}
}

void SemanticValidationPass::visit(BlockStatement* node)
{
	for (auto& statement : node->statements)
	{
		AstVisitor::visit(statement.get());
	}
}

void SemanticValidationPass::visit(VariableStatement* node)
{
	for (size_t i = 0; i < node->names.size(); i++)
	{
		if (localVariables.contains(node->names[i]))
			reportError(node, "Variable is already defined");
		AstVisitor::visit(node->values[i].get());
		localVariables.try_emplace(node->names[i], expressionResult);
	}
}

void SemanticValidationPass::visit(ReturnStatement* node)
{
	AstVisitor::visit(node->expression.get());
	if (!Type::areSame(expressionResult, functionResult))
		reportError(node->expression.get(), "Return expression has to be of function result type");
}

void SemanticValidationPass::visit(WhileStatement* node)
{
	AstVisitor::visit(node->condition.get());
	if (!Type::areSame(expressionResult, typeCtx.getNamedType("bool")))
		reportError(node->condition.get(), "While condition has to be of boolean type");
	
	AstVisitor::visit(node->body.get());
}

void SemanticValidationPass::visit(IfStatement* node)
{
	AstVisitor::visit(node->condition.get());
	if (!Type::areSame(expressionResult, typeCtx.getNamedType("bool")))
		reportError(node->condition.get(), "If condition has to be of boolean type");

	AstVisitor::visit(node->ifBody.get());
	if (node->elseBody) AstVisitor::visit(node->elseBody.get());
}

void SemanticValidationPass::visit(FunctionDeclaration* node)
{
	// create vector if this is the first function called what it is called
	if (!functions.contains(node->name))
		functions.try_emplace(node->name, std::vector<FunctionDeclaration>());
	
	std::vector<Type*> args;
	for (auto& param : node->parameters)
	{
		args.push_back(param.second);
	}

	if (hasFunction(node->name, args))
		reportError(node, "Function is already defined");

	// add function to known functions
	functions.at(node->name).push_back(*node);
}

void SemanticValidationPass::visit(Module* node)
{
	for (auto& declaration : node->declarations)
	{
		AstVisitor::visit(declaration.get());
	}
}

bool SemanticValidationPass::hasFunction(std::string const& name, std::vector<Type*> const& args)
{
	if (!functions.contains(name))
		return false;

	for (auto& candidate : functions.at(name))
	{
		// parameter count differs, no match found
		if (candidate.parameters.size() != args.size())
			continue;

		for (size_t i = 0; i < candidate.parameters.size(); i++)
		{
			// parameter is different, no match found
			if (!Type::areSame(candidate.parameters[i].second, args[i]))
				break;

			if (i == candidate.parameters.size() - 1)
			{
				return true;
			}
		}
	}

	return false;
}

FunctionDeclaration const& SemanticValidationPass::getFunction(std::string const& name, std::vector<Type*> const& args)
{
	if (!functions.contains(name))
		throw std::exception();

	for (auto& candidate : functions.at(name))
	{
		// parameter count differs, no match found
		if (candidate.parameters.size() != args.size())
			continue;

		for (size_t i = 0; i < candidate.parameters.size(); i++)
		{
			// parameter is different, no match found
			if (!Type::areSame(candidate.parameters[i].second, args[i]))
				break;

			if (i == candidate.parameters.size() - 1)
			{
				return candidate;
			}
		}
	}

	throw std::exception("No matching function");
}

void SemanticValidationPass::reportError(AstNode* node, std::string msg)
{
	size_t line = 1, column = 1;
	for (size_t i = 0; i < node->begin && i < source.length(); i++) 
	{
		if (source[i] == '\n') 
		{
			line++;
			column = 1;
		} 
		else 
		{
			column++;
		}
	}

	logStream << "ln " << line << ", col " << column << ", \"" << source.substr(node->begin, node->end - node->begin) << "\": " << msg << "\n";
	throw std::exception(msg.c_str());
}