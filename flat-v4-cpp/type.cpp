#include "type.hpp"

bool Type::areSame(Type* a, Type* b)
{
	return a->getResolvedType()->isSame(b->getResolvedType());
}

Type* TypeContext::getNamedType(std::string const& name)
{
	if (!namedTypes.contains(name))
		namedTypes.try_emplace(name, new NamedType(*this, name));
	return namedTypes.at(name);
}

Type* TypeContext::getPointerType(Type* base)
{
	if (!pointerTypes.contains(base))
		pointerTypes.try_emplace(base, new PointerType(base));
	return pointerTypes.at(base);
}

Type* TypeContext::getArrayType(Type* base)
{
	if (!arrayTypes.contains(base))
		arrayTypes.try_emplace(base, new ArrayType(base));
	return arrayTypes.at(base);
}

Type* TypeContext::resolveNamedType(std::string const& name)
{
	if (builtinTypes.contains(name))
		return builtinTypes.at(name);
	else if (structTypes.contains(name))
		return structTypes.at(name);

	throw std::exception("Unknown type");
}

size_t BuiltinType::getBitSize()
{
	return bitSize;
}

bool BuiltinType::isSame(Type* other)
{
	return dynamic_cast<BuiltinType*>(other) 
		&& name == dynamic_cast<BuiltinType*>(other)->name 
		&& bitSize == dynamic_cast<BuiltinType*>(other)->bitSize;
}

Type* BuiltinType::getResolvedType()
{
	return this;
}

std::string BuiltinType::toString()
{
	return name;
}

size_t StructType::getBitSize()
{
	size_t bitSize = 0;
	for (auto& [name, type] : members)
	{
		bitSize += type->getBitSize();
		bitSize = (size_t)(ceil(bitSize / (double)ctx.pointerSize) * ctx.pointerSize); // align bitSize to pointer size
	}

	return bitSize;
}

bool StructType::isSame(Type* other)
{
	auto o = dynamic_cast<StructType const*>(other);
	if (!o)
		return false;

	if (members.size() != o->members.size())
		return false;

	for (size_t i = 0; i < members.size(); i++)
	{
		if (!Type::areSame(members[i].second, o->members[i].second))
			return false;
	}

	return true;
}

Type* StructType::getResolvedType()
{
	return this;
}

std::string StructType::toString()
{
	return name;
}

size_t PointerType::getBitSize()
{
	return ctx.pointerSize;
}

bool PointerType::isSame(Type* other)
{
	return dynamic_cast<PointerType const*>(other) 
		&& Type::areSame(base, dynamic_cast<PointerType const*>(other)->base);
}

Type* PointerType::getResolvedType()
{
	return this;
}

std::string PointerType::toString()
{
	return base->toString() + "*";
}

size_t ArrayType::getBitSize()
{
	return ctx.pointerSize;
}

bool ArrayType::isSame(Type* other)
{
	return dynamic_cast<ArrayType const*>(other) 
		&& Type::areSame(base, dynamic_cast<ArrayType const*>(other)->base);
}

Type* ArrayType::getResolvedType()
{
	return this;
}

std::string ArrayType::toString()
{
	return base->toString() + "[]";
}

size_t NamedType::getBitSize()
{
	return getResolvedType()->getBitSize();
}

bool NamedType::isSame(Type* other)
{
	return Type::areSame(getResolvedType(), other);
}

Type* NamedType::getResolvedType()
{
	return ctx.resolveNamedType(name);
}

std::string NamedType::toString()
{
	return name;
}