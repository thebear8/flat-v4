#include "ast.hpp"

void AstVisitor::visit(AstNode* node)
{
	node->accept(this);
}