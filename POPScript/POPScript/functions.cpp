#include "functions.h"

Callable::Parameter::Parameter() :
	_type{},
	_name{}
{}
Callable::Parameter::Parameter(const DataType& type, const std::string& name) :
	_type{ type },
	_name{ name }
{}

const DataType& Callable::Parameter::type() const { return _type; }
const std::string& Callable::Parameter::name() const { return _name; }









Callable::Callable() :
	_type{ Type::Invalid },
	_name{},
	_pars{},
	_void{},
	_code{}
{}
Callable::Callable(Type type, const std::string& name, bool isVoid, CodeValue code, const std::vector<Parameter>& parameters) :
	_type{ type },
	_name{ name },
	_pars{ parameters },
	_void{ isVoid },
	_code{ code }
{}
Callable::Callable(const Callable& c) :
	_type{ c._type },
	_name{ c._name },
	_pars{ c._pars },
	_void{ c._void },
	_code{ c._code }
{}
Callable::Callable(Callable&& c) noexcept :
	_type{ std::move(c._type) },
	_name{ std::move(c._name) },
	_pars{ std::move(c._pars) },
	_void{ std::move(c._void) },
	_code{ std::move(c._code) }
{}
Callable::~Callable() {}

Callable& Callable::operator= (const Callable& c)
{
	_type = c._type;
	_name = c._name;
	_pars = c._pars;
	_void = c._void;
	_code = c._code;
	return *this;
}
Callable& Callable::operator= (Callable&& c) noexcept
{
	_type = std::move(c._type);
	_name = std::move(c._name);
	_pars = std::move(c._pars);
	_void = std::move(c._void);
	_code = std::move(c._code);
	return *this;
}

const std::string& Callable::name() const { return _name; }

size_t Callable::parameterCount() const { return _pars.size(); }
const Callable::Parameter& Callable::parameter(size_t idx) const { return _pars[idx]; }

bool Callable::isVoid() const { return _void; }

const Callable::Parameter& Callable::operator[] (size_t idx) const { return _pars[idx]; }

CodeValue Callable::code() const { return _code; }

bool Callable::isGetter() const { return _type == Type::Getter; }
bool Callable::isSetter() const { return _type == Type::Setter; }
bool Callable::isFunction() const { return _type == Type::Function; }

bool operator== (const Callable& c0, const Callable& c1) { return c0._code == c1._code; }
bool operator!= (const Callable& c0, const Callable& c1) { return c0._code != c1._code; }

Callable Callable::getter(const std::string& name, CodeValue code)
{
	return { Type::Getter, name, false, code, {} };
}
Callable Callable::setter(const std::string& name, CodeValue code, DataType valueType)
{
	return { Type::Setter, name, true, code, { { valueType, "value" } } };
}
Callable Callable::function(const std::string& name, CodeValue code, bool isVoid, const std::vector<Parameter>& parameters)
{
	return { Type::Function, name, isVoid, code, parameters };
}
