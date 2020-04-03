#pragma once

#include "utils.h"

class LangElement : public Conversor<LangElement>
{
public:
	enum class Type
	{
		Namespace,
		Class,
		Object,
		Attribute,
		ReadOnlyAttribute,
		Function
	};

private:
	LangElement* _parent;
	std::string _name;

public:
	LangElement();
	virtual ~LangElement();

	const LangElement& parent() const;
	bool hasParent() const;

	const std::string& name() const;

	virtual Type type() const = 0;

	LangElement(const LangElement&) = delete;
	LangElement& operator= (const LangElement&) = delete;

protected:
	void setParent(LangElement* parent);
	LangElement& parent();

	void setName(const std::string& name);

public:
	inline bool isNamespace() const { return type() == Type::Namespace; }
	inline bool isClass() const { return type() == Type::Class; }
	inline bool isObject() const { return type() == Type::Object; }
	inline bool isAttribute() const { return type() == Type::Attribute; }
	inline bool isReadOnlyAttribute() const { return type() == Type::ReadOnlyAttribute; }
	inline bool isFunction() const { return type() == Type::Function; }
};



class Namespace : public LangElement
{
private:
	std::vector<LangElement*> _children;

public:
	Namespace();
	virtual ~Namespace();
};

