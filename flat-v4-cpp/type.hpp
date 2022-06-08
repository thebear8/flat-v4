#pragma once
#include <memory>
#include <string>
#include <map>
#include <unordered_map>

class TypeContext;

class Type
{
public:
	TypeContext& ctx;

public:
	Type(TypeContext& ctx) :
		ctx(ctx)
	{
	}

public:
	virtual size_t getBitSize() = 0;
	virtual bool isSame(Type* other) = 0;
	virtual Type* getResolvedType() = 0;
	virtual std::string toString() = 0;

public:
	static bool areSame(Type* a, Type* b);
};

class TypeContext
{
public:
	size_t pointerSize;

	std::unordered_map<std::string, Type*> namedTypes;
	std::unordered_map<std::string, Type*> builtinTypes;
	std::unordered_map<std::string, Type*> structTypes;
	std::unordered_map<Type*, Type*> pointerTypes;
	std::unordered_map<Type*, Type*> arrayTypes;

public:
	TypeContext(size_t pointerSize) :
		pointerSize(pointerSize)
	{
	}

public:
	Type* getNamedType(std::string const& name);
	Type* getPointerType(Type* base);
	Type* getArrayType(Type* base);
	Type* resolveNamedType(std::string const& name);
};

class BuiltinType : public Type
{
public:
	std::string name;
	size_t bitSize;

public:
	BuiltinType(TypeContext& ctx, std::string name, size_t bitSize) :
		Type(ctx),
		name(name),
		bitSize(bitSize)
	{
	}

public:
	virtual size_t getBitSize() override;
	virtual bool isSame(Type* other) override;
	virtual Type* getResolvedType() override;
	virtual std::string toString() override;
};

class StructType : public Type
{
public:
	std::string name;
	std::vector<std::pair<std::string, Type*>> members;

public:
	StructType(TypeContext& ctx, std::string name, std::vector<std::pair<std::string, Type*>> members) :
		Type(ctx),
		name(name),
		members(members)
	{
	}

public:
	virtual size_t getBitSize() override;
	virtual bool isSame(Type* other) override;
	virtual Type* getResolvedType() override;
	virtual std::string toString() override;
};

class PointerType : public Type
{
public:
	Type* base;

public:
	PointerType(Type* base) :
		Type(base->ctx),
		base(base)
	{
	}

public:
	virtual size_t getBitSize() override;
	virtual bool isSame(Type* other) override;
	virtual Type* getResolvedType() override;
	virtual std::string toString() override;
};

class ArrayType : public Type
{
public:
	Type* base;

public:
	ArrayType(Type* base) :
		Type(base->ctx),
		base(base)
	{
	}

public:
	virtual size_t getBitSize() override;
	virtual bool isSame(Type* other) override;
	virtual Type* getResolvedType() override;
	virtual std::string toString() override;
};

class NamedType : public Type
{
public:
	std::string name;

public:
	NamedType(TypeContext& ctx, std::string name) :
		Type(ctx),
		name(name)
	{
	}

public:
	virtual size_t getBitSize() override;
	virtual bool isSame(Type* other) override;
	virtual Type* getResolvedType() override;
	virtual std::string toString() override;
};