#pragma once

#include <vector>

#include "types.h"
#include "utils.h"

class Callable
{
public:
	class Parameter
	{
	private:
		DataType _type;
		std::string _name;

	public:
		Parameter();
		Parameter(const DataType& type, const std::string& name);
		Parameter(const Parameter&) = default;
		Parameter(Parameter&&) noexcept = default;

		Parameter& operator= (const Parameter&) = default;
		Parameter& operator= (Parameter&&) noexcept = default;

		const DataType& type() const;
		const std::string& name() const;
	};

	enum class Type
	{
		Invalid,
		Getter,
		Setter,
		Function
	};

private:
	Type _type;

	std::string _name;
	std::vector<Parameter> _pars;
	bool _void;

	CodeValue _code;

public:
	Callable();
	Callable(const Callable& c);
	Callable(Callable&& c) noexcept;
	~Callable();

	Callable& operator= (const Callable& c);
	Callable& operator= (Callable&& c) noexcept;

	const std::string& name() const;

	size_t parameterCount() const;
	const Parameter& parameter(size_t idx) const;

	bool isVoid() const;

	const Parameter& operator[] (size_t idx) const;

	CodeValue code() const;

	bool isGetter() const;
	bool isSetter() const;
	bool isFunction() const;

	friend bool operator== (const Callable& c0, const Callable& c1);
	friend bool operator!= (const Callable& c0, const Callable& c1);

private:
	Callable(Type type, const std::string& name, bool isVoid, CodeValue code, const std::vector<Parameter>& parameters);

public:
	static Callable getter(const std::string& name, CodeValue code);
	static Callable setter(const std::string& name, CodeValue code, DataType valueType = DataType::integer());
	static Callable function(const std::string& name, CodeValue code, bool isVoid, const std::vector<Parameter>& parameters);
};

