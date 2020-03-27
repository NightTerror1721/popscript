#include "parser_elements.h"

#include <cstdarg>
#include <sstream>



static inline std::string ident(const size_t identation)
{
	return !identation ? std::string{ "" } : std::string(identation, ' ');
}




bool CodeFragment::is(CodeFragmentType type) const { return getCodeFragmentType() == type; }

std::ostream& operator<< (std::ostream& os, const CodeFragment& cf) { return os << cf.toString(); }




bool Statement::isStatement() const { return true; }





Identifier::Identifier(const std::string& identifier) :
	Statement{},
	_id{ identifier }
{
	if (!isValid(identifier))
		throw InvalidIdentifier{};
}
Identifier::Identifier(const Identifier& id) :
	Statement{ id },
	_id{ id._id }
{}
Identifier::Identifier(Identifier&& id) noexcept :
	Statement{ std::move(id) },
	_id{ std::move(id._id) }
{}
Identifier::~Identifier() {}

Identifier& Identifier::operator= (const Identifier& id)
{
	_id = id._id;
	return *this;
}
Identifier& Identifier::operator= (Identifier&& id) noexcept
{
	_id = std::move(id._id);
	return *this;
}

CodeFragmentType Identifier::getCodeFragmentType() const { return CodeFragmentType::Identifier; }

std::string Identifier::toString(size_t identation) const { return _id; }

void* Identifier::clone() const { return new Identifier{ _id }; }

bool Identifier::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Identifier))
		return operator==(reinterpret_cast<const Identifier&>(c));
	return false;
}
bool Identifier::operator== (const Identifier& id) const { return _id == id._id; }
bool Identifier::operator!= (const Identifier& id) const { return _id != id._id; }

const std::regex Identifier::_pattern{ "[_a-zA-Z][_a-zA-Z0-9]*" };

bool Identifier::isValid(const std::string& str) { return std::regex_search(str, _pattern); }






LiteralInteger::LiteralInteger(FieldValue value) :
	Statement{},
	_value{ value }
{}
LiteralInteger::LiteralInteger(const LiteralInteger& lit) :
	Statement{ lit },
	_value{ lit._value }
{}
LiteralInteger::LiteralInteger(LiteralInteger&& lit) noexcept :
	Statement{ std::move(lit) },
	_value{ std::move(lit._value) }
{}
LiteralInteger::~LiteralInteger() {}

LiteralInteger& LiteralInteger::operator= (const LiteralInteger& lit)
{
	_value = lit._value;
	return *this;
}
LiteralInteger& LiteralInteger::operator= (LiteralInteger&& lit) noexcept
{
	_value = std::move(lit._value);
	return *this;
}

FieldValue LiteralInteger::getValue() const { return _value; }

CodeFragmentType LiteralInteger::getCodeFragmentType() const { return CodeFragmentType::LiteralInteger; }

std::string LiteralInteger::toString(size_t identation) const { return std::to_string(_value); }

void* LiteralInteger::clone() const { return new LiteralInteger{ _value }; }

bool LiteralInteger::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::LiteralInteger))
		return operator==(reinterpret_cast<const LiteralInteger&>(c));
	return false;
}
bool LiteralInteger::operator== (const LiteralInteger& lit) const { return _value == lit._value; }
bool LiteralInteger::operator!= (const LiteralInteger& lit) const { return _value != lit._value; }

const std::regex LiteralInteger::_pattern{ "(0|0[xX])?[0-9]+" };

static int find_integer_base(const std::string& str)
{
	if (str.size() <= 1 || str[0] != '0')
		return 10;
	const char& c = str[1];
	return c == 'x' || c == 'X' ? 16 : 8;
}

LiteralInteger LiteralInteger::parse(const std::string& str)
{
	return std::stol(str, nullptr, find_integer_base(str));
}
bool LiteralInteger::isValid(const std::string& str) { return std::regex_search(str, _pattern); }








TypeConstant::TypeConstant(ScriptCode value) :
	Statement{},
	_value{ value },
	_type{ DataType::findTypeFromValue(value) }
{}
TypeConstant::TypeConstant(const TypeConstant& tc) :
	Statement{ tc },
	_value{ tc._value },
	_type{ tc._type }
{}
TypeConstant::TypeConstant(TypeConstant&& tc) noexcept :
	Statement{ std::move(tc) },
	_value{ std::move(tc._value) },
	_type{ std::move(tc._type) }
{}
TypeConstant::~TypeConstant() {}

TypeConstant& TypeConstant::operator= (const TypeConstant& tc)
{
	_type = tc._type;
	_value = tc._value;
	return *this;
}
TypeConstant& TypeConstant::operator= (TypeConstant&& tc) noexcept
{
	_type = std::move(tc._type);
	_value = std::move(tc._value);
	return *this;
}

bool TypeConstant::isValid() const { return _type.isValid(); }

DataType TypeConstant::getType() const { return _type; }
ScriptCode TypeConstant::getValue() const { return _value; }

CodeFragmentType TypeConstant::getCodeFragmentType() const { return CodeFragmentType::TypeConstant; }

std::string TypeConstant::toString(size_t identation) const
{
	return _type ? _type.getValueIdentifier(_value) : std::to_string(_value);
}

void* TypeConstant::clone() const { return new TypeConstant{ *this }; }

bool TypeConstant::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::TypeConstant))
		return operator==(reinterpret_cast<const TypeConstant&>(c));
	return false;
}
bool TypeConstant::operator== (const TypeConstant& tc) const { return _value == tc._value; }
bool TypeConstant::operator!= (const TypeConstant& tc) const { return _value != tc._value; }


bool TypeConstant::isValid(const std::string& str) { return DataType::findTypeFromValueName(str); }
bool TypeConstant::isValid(ScriptCode code) { return DataType::findTypeFromValue(code); }









Stopchar::Stopchar(char symbol) :
	CodeFragment{},
	_symbol{ symbol }
{}
Stopchar::Stopchar(const Stopchar& sc) :
	CodeFragment{ sc },
	_symbol{ sc._symbol }
{}
Stopchar::~Stopchar() {}

Stopchar& Stopchar::operator= (const Stopchar& sc)
{
	_symbol = sc._symbol;
	return *this;
}

CodeFragmentType Stopchar::getCodeFragmentType() const { return CodeFragmentType::Stopchar; }

bool Stopchar::isStatement() const { return false; }

std::string Stopchar::toString(size_t identation) const { return std::string{ _symbol }; }

void* Stopchar::clone() const { return new Stopchar{ *this }; }

bool Stopchar::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Stopchar))
		return operator==(reinterpret_cast<const Stopchar&>(c));
	return false;
}
bool Stopchar::operator== (const Stopchar& sc) const { return _symbol == sc._symbol; }
bool Stopchar::operator!= (const Stopchar& sc) const { return _symbol != sc._symbol; }

const Stopchar Stopchar::Semicolon{ ';' };
const Stopchar Stopchar::Comma{ ',' };
const Stopchar Stopchar::TwoPoints{ ':' };









ArgumentList::ArgumentList() :
	_args{}
{}
ArgumentList::ArgumentList(const ArgumentList& a) :
	_args{ a._args }
{}
ArgumentList::ArgumentList(ArgumentList&& a) noexcept :
	_args{ std::move(a._args) }
{}
ArgumentList::~ArgumentList() {}

ArgumentList& ArgumentList::operator= (const ArgumentList& a)
{
	_args = a._args;
	return *this;
}
ArgumentList& ArgumentList::operator= (ArgumentList&& a) noexcept
{
	_args = std::move(a._args);
	return *this;
}

bool ArgumentList::empty() const { return _args.empty(); }
size_t ArgumentList::size() const { return _args.size(); }

void ArgumentList::addArgument(const Statement& arg) { _args.push_back(arg); }

const Statement& ArgumentList::operator[] (const size_t idx) const { return _args[idx]; }

std::string ArgumentList::toString() const
{
	if (_args.empty())
		return "()";

	std::stringstream ss;
	ss << "(";

	const size_t len = _args.size();
	_args.for_each([&ss, len](const Statement& s, size_t index) {
		if (index + 1 >= len)
			ss << s.toString() << ")";
		else ss << s.toString() << " ,";
	});

	return ss.str();
}

bool ArgumentList::operator== (const ArgumentList& al) const { return _args == al._args; }
bool ArgumentList::operator!= (const ArgumentList& al) const { return _args != al._args; }









FunctionArguments::FunctionArguments() :
	Statement{},
	ArgumentList{}
{}
FunctionArguments::FunctionArguments(const FunctionArguments& a) :
	Statement{ a },
	ArgumentList{ a }
{}
FunctionArguments::FunctionArguments(FunctionArguments&& a) noexcept :
	Statement{ std::move(a) },
	ArgumentList{ std::move(a) }
{}
FunctionArguments::~FunctionArguments() {}

FunctionArguments& FunctionArguments::operator= (const FunctionArguments& a)
{
	ArgumentList::operator=(a);
	return *this;
}
FunctionArguments& FunctionArguments::operator= (FunctionArguments&& a) noexcept
{
	ArgumentList::operator=(std::move(a));
	return *this;
}

CodeFragmentType FunctionArguments::getCodeFragmentType() const { return CodeFragmentType::FunctionArguments; }

std::string FunctionArguments::toString(size_t identation) const { return ArgumentList::toString(); }

void* FunctionArguments::clone() const { return new FunctionArguments{ *this }; }

bool FunctionArguments::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::FunctionArguments))
		return operator==(reinterpret_cast<const FunctionArguments&>(c));
	return false;
}
bool FunctionArguments::operator== (const FunctionArguments& a) const { return ArgumentList::operator==(a); }
bool FunctionArguments::operator!= (const FunctionArguments& a) const { return ArgumentList::operator!=(a); }









CommandArguments::CommandArguments() :
	CodeFragment{},
	ArgumentList{}
{}
CommandArguments::CommandArguments(const CommandArguments& a) :
	CodeFragment{ a },
	ArgumentList{ a }
{}
CommandArguments::CommandArguments(CommandArguments&& a) noexcept :
	CodeFragment{ std::move(a) },
	ArgumentList{ std::move(a) }
{}
CommandArguments::~CommandArguments() {}

CommandArguments& CommandArguments::operator= (const CommandArguments& a)
{
	ArgumentList::operator=(a);
	return *this;
}
CommandArguments& CommandArguments::operator= (CommandArguments&& a) noexcept
{
	ArgumentList::operator=(std::move(a));
	return *this;
}

CodeFragmentType CommandArguments::getCodeFragmentType() const { return CodeFragmentType::CommandArguments; }

bool CommandArguments::isStatement() const { return false; }

std::string CommandArguments::toString(size_t identation) const { return ArgumentList::toString(); }

void* CommandArguments::clone() const { return new CommandArguments{ *this }; }

bool CommandArguments::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::CommandArguments))
		return operator==(reinterpret_cast<const CommandArguments&>(c));
	return false;
}
bool CommandArguments::operator== (const CommandArguments& a) const { return ArgumentList::operator==(a); }
bool CommandArguments::operator!= (const CommandArguments& a) const { return ArgumentList::operator!=(a); }










Operator::Operator(const std::string& symbol, const Type type, unsigned int priority, bool rightToLeft, bool conditional) :
	CodeFragment{},
	_id{},
	_symbol{ symbol },
	_type{ type },
	_priority{ priority },
	_rightToLeft{ rightToLeft },
	_conditional{ conditional }
{
	uint8_t id_gen = 0;
	_id = id_gen++;
}
Operator::Operator(const Operator& op) :
	CodeFragment{ op },
	_id{ op._id },
	_symbol{ op._symbol },
	_type{ op._type },
	_priority{ op._priority },
	_rightToLeft{ op._rightToLeft },
	_conditional{ op._conditional }
{}
Operator::Operator(Operator&& op) noexcept :
	CodeFragment{ std::move(op) },
	_id{ std::move(op._id) },
	_symbol{ std::move(op._symbol) },
	_type{ std::move(op._type) },
	_priority{ std::move(op._priority) },
	_rightToLeft{ std::move(op._rightToLeft) },
	_conditional{ std::move(op._conditional) }
{}
Operator::~Operator() {}

Operator& Operator::operator= (const Operator& op)
{
	_id = op._id;
	_symbol = op._symbol;
	_type = op._type;
	_priority = op._priority;
	_rightToLeft = op._rightToLeft;
	_conditional = op._conditional;
	return *this;
}
Operator& Operator::operator= (Operator&& op) noexcept
{
	_id = std::move(op._id);
	_symbol = std::move(op._symbol);
	_type = std::move(op._type);
	_priority = std::move(op._priority);
	_rightToLeft = std::move(op._rightToLeft);
	_conditional = std::move(op._conditional);
	return *this;
}

unsigned int Operator::getPriority() const { return _priority; }
bool Operator::hasRightToLeft() const { return _rightToLeft; }
bool Operator::isConditional() const { return _conditional; }

bool Operator::isUnary() const { return _type == Type::Unary; }
bool Operator::isBinary() const { return _type == Type::Binary; }
bool Operator::isTernary() const { return _type == Type::Ternary; }
bool Operator::isAssignment() const { return _type == Type::Assignment; }

int Operator::comparePriority(const Operator& other) const
{
	if (_priority == other._priority)
		return _rightToLeft || other._rightToLeft ? -1 : 0;
	return _priority < other._priority ? 1 : -1;
}

CodeFragmentType Operator::getCodeFragmentType() const { return CodeFragmentType::Operator; }

bool Operator::isStatement() const { return false; }

std::string Operator::toString(size_t identation) const { return _symbol; }

void* Operator::clone() const { return new Operator{ *this }; }

bool Operator::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Operator))
		return operator==(reinterpret_cast<const Operator&>(c));
	return false;
}
bool Operator::operator== (const Operator& op) const { return _id == op._id; }
bool Operator::operator!= (const Operator& op) const { return _id != op._id; }

const Operator Operator::SufixIncrement{ "++", Operator::Type::Unary, 0, false, false };
const Operator Operator::SufixDecrement{ "--", Operator::Type::Unary, 0, false, false };

const Operator Operator::PrefixIncrement{ "++", Operator::Type::Unary, 1, true, false };
const Operator Operator::PrefixDecrement{ "--", Operator::Type::Unary, 1, true, false };
const Operator Operator::UnaryMinus{ "-", Operator::Type::Unary, 1, true, false };
const Operator Operator::BinaryNot{ "!", Operator::Type::Unary, 1, true, false };

const Operator Operator::Multiplication{ "*", Operator::Type::Binary, 2, false, false };
const Operator Operator::Division{ "/", Operator::Type::Binary, 2, false, false };

const Operator Operator::Addition{ "+", Operator::Type::Binary, 3, false, false };
const Operator Operator::Subtraction{ "-", Operator::Type::Binary, 3, false, false };

const Operator Operator::GreaterThan{ ">", Operator::Type::Binary, 4, false, true };
const Operator Operator::SmallerThan{ "<", Operator::Type::Binary, 4, false, true };
const Operator Operator::GreaterEqualsThan{ ">=", Operator::Type::Binary, 4, false, true };
const Operator Operator::SmallerEqualsThan{ "<=", Operator::Type::Binary, 4, false, true };

const Operator Operator::EqualsTo{ "==", Operator::Type::Binary, 5, false, true };
const Operator Operator::NotEqualsTo{ "!=", Operator::Type::Binary, 5, false, true };

const Operator Operator::BinaryAnd{ "&&", Operator::Type::Binary, 6, false, false };
const Operator Operator::BinaryOr{ "||", Operator::Type::Binary, 6, false, false };

const Operator Operator::TernaryConditional{ "?:", Operator::Type::Ternary, 7, false, false };

const Operator Operator::Assignment{ "=", Operator::Type::Assignment, 8, true, false };
const Operator Operator::AssignmentAddition{ "+=", Operator::Type::Assignment, 8, true, false };
const Operator Operator::AssignmentSubtraction{ "-=", Operator::Type::Assignment, 8, true, false };
const Operator Operator::AssignmentMultiplication{ "*=", Operator::Type::Assignment, 8, true, false };
const Operator Operator::AssignmentDivision{ "/=", Operator::Type::Assignment, 8, true, false };











Operation::BadOperation::BadOperation(const char* msg) :
	exception{ msg }
{}

Operation::Operation(const Operator& op, const Statement& operand0, const Statement* operand1, const Statement* operand2) :
	Statement{},
	_operator{ op },
	_operands{ operand0, operand1, operand2 }
{}
Operation::Operation(const Operation& o) :
	Statement{ o },
	_operator{ o._operator },
	_operands{ o._operands[0], o._operands[1], o._operands[2] }
{}
Operation::Operation(Operation&& o) noexcept :
	Statement{ std::move(o) },
	_operator{ std::move(o._operator) },
	_operands{ std::move(o._operands[0]), std::move(o._operands[1]), std::move(o._operands[2]) }
{}
Operation::~Operation() {}

Operation& Operation::operator= (const Operation& o)
{
	_operator = o._operator;
	_operands[0] = o._operands[0];
	_operands[1] = o._operands[1];
	_operands[2] = o._operands[2];
	return *this;
}
Operation& Operation::operator= (Operation&& o) noexcept
{
	_operator = std::move(o._operator);
	_operands[0] = std::move(o._operands[0]);
	_operands[1] = std::move(o._operands[1]);
	_operands[2] = std::move(o._operands[2]);
	return *this;
}

bool Operation::isUnary() const { return _operator.isUnary(); }
bool Operation::isBinary() const { return _operator.isBinary(); }
bool Operation::isTernary() const { return _operator.isTernary(); }
bool Operation::isAssignment() const { return _operator.isAssignment(); }

const Operator& Operation::getOperator() const { return _operator; }

size_t Operation::getOperandCount() const
{
	return _operator.isUnary() ? 1 : _operator.isTernary() ? 3 : 2;
}
const Statement& Operation::getOperand(const size_t idx) const { return _operands[idx]; }

CodeFragmentType Operation::getCodeFragmentType() const { return CodeFragmentType::Operation; }

std::string Operation::toString(size_t identation) const
{
	if (_operator.isUnary())
	{
		return _operator == Operator::SufixIncrement || _operator == Operator::SufixDecrement
			? _operands[0]->toString() + _operator.toString()
			: _operator.toString() + _operands[0]->toString();
	}
	if (_operator.isTernary())
		return _operands[0]->toString() + " ? " + _operands[1]->toString() + " : " + _operands[2]->toString();

	return _operands[0]->toString() + " " + " " + _operator.toString() + _operands[1]->toString();
}

void* Operation::clone() const { return new Operation{ *this }; }

bool Operation::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Operation))
		return operator==(reinterpret_cast<const Operation&>(c));
	return false;
}
bool Operation::operator== (const Operation& o) const { return _operator == o._operator && _operands[0] == o._operands[0] && _operands[1] == o._operands[1] && _operands[2] == o._operands[2]; }
bool Operation::operator!= (const Operation& o) const { return _operator != o._operator || _operands[0] != o._operands[0] || _operands[1] != o._operands[1] || _operands[2] != o._operands[2]; }

Operation Operation::unary(const Operator& op, const Statement& operand)
{
	if (!op.isUnary())
		throw BadOperation{ "Required a Unary operator in a Unary Operation" };
	if (!operand.is(CodeFragmentType::Identifier))
		throw BadOperation{ "Expected a valid Identifier in Unary Operator" };
	return { op, operand };
}
Operation Operation::binary(const Operator& op, const Statement& left, const Statement& right)
{
	if (!op.isBinary())
		throw BadOperation{ "Required a Binary operator in a Binary Operation" };
	return { op, left, &right };
}
Operation Operation::ternary(const Statement& cond, const Statement& opIfTrue, const Statement& opIfFalse)
{
	return { Operator::TernaryConditional, cond, &opIfTrue, &opIfFalse };
}
Operation Operation::assignment(const Operator& op, const Statement& left, const Statement& right)
{
	if (!op.isAssignment())
		throw BadOperation{ "Required a Assignment operator in an Assignment Operation" };
	if (!left.is(CodeFragmentType::Identifier))
		throw BadOperation{ "Expected a valid Identifier in left part of Assignment operator" };
	return { op, left, &right };
}













FunctionCall::BadFunctionCall::BadFunctionCall(const char* msg) :
	exception{ msg }
{}

FunctionCall::FunctionCall() :
	_callable{ nullptr },
	_args{}
{}
FunctionCall::FunctionCall(const Callable& callable, const FunctionArguments& args) :
	_callable{ &callable },
	_args{ args }
{}
FunctionCall::FunctionCall(const FunctionCall& fc) :
	_callable{ fc._callable },
	_args{ fc._args }
{}
FunctionCall::FunctionCall(FunctionCall&& fc) noexcept :
	_callable{ std::move(fc._callable) },
	_args{ std::move(fc._args) }
{}
FunctionCall::~FunctionCall() {}

FunctionCall& FunctionCall::operator= (const FunctionCall& fc)
{
	_callable = fc._callable;
	_args = fc._args;
	return *this;
}
FunctionCall& FunctionCall::operator= (FunctionCall&& fc) noexcept
{
	_callable = std::move(fc._callable);
	_args = std::move(fc._args);
	return *this;
}

const Callable& FunctionCall::getCallable() const { return *_callable; }
const FunctionArguments& FunctionCall::getArguments() const { return _args; }

CodeFragmentType FunctionCall::getCodeFragmentType() const { return CodeFragmentType::FunctionCall; }

std::string FunctionCall::toString(size_t identation) const { return _callable->name() + _args.toString(); }

void* FunctionCall::clone() const { return new FunctionCall{ *this }; }

bool FunctionCall::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::FunctionCall))
		return operator==(reinterpret_cast<const FunctionCall&>(c));
	return false;
}
bool FunctionCall::operator== (const FunctionCall& fc) const
{
	if (!_callable)
		return !fc._callable;
	return fc._callable && *_callable == *fc._callable && _args == fc._args;
}
bool FunctionCall::operator!= (const FunctionCall& fc) const
{
	if (!_callable)
		return fc._callable;
	return !fc._callable || *_callable != *fc._callable || _args != fc._args;
}

FunctionCall FunctionCall::make(const Callable& callable, const Statement& args)
{
	if (!args.is(CodeFragmentType::FunctionArguments))
		throw BadFunctionCall{ "Expected valid Function Arguments for FunctionCall statement" };
	return { callable, reinterpret_cast<const FunctionArguments&>(args) };
}










Command::Command() :
	_id{ 0 },
	_name{}
{}
Command::Command(const std::string& name) :
	_id{},
	_name{ name }
{
	static uint8_t id_gen = 0;
	_id = ++id_gen;
}
Command::Command(const Command& c) :
	_id{ c._id },
	_name{ c._name }
{}
Command::Command(Command&& c) noexcept :
	_id{ std::move(c._id) },
	_name{ std::move(c._name) }
{}
Command::~Command() {}

Command& Command::operator= (const Command& c)
{
	_id = c._id;
	_name = c._name;
	return *this;
}
Command& Command::operator= (Command&& c) noexcept
{
	_id = std::move(c._id);
	_name = std::move(c._name);
	return *this;
}

CodeFragmentType Command::getCodeFragmentType() const { return CodeFragmentType::Command; }

bool Command::isStatement() const { return false; }

std::string Command::toString(size_t identation) const { return _name; }

void* Command::clone() const { return new Command{ *this }; }

bool Command::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Command))
		return operator==(reinterpret_cast<const Command&>(c));
	return false;
}
bool Command::operator== (const Command& c) const { return _id == c._id; }
bool Command::operator!= (const Command& c) const { return _id != c._id; }

const Command Command::Invalid;
const Command Command::Var{ "var" };
const Command Command::Const{ "const" };
const Command Command::Define{ "define" };
const Command Command::Import{ "import" };
const Command Command::If{ "if" };
const Command Command::Else{ "else" };
const Command Command::Every{ "every" };












Instruction::Instruction() :
	CodeFragment{}
{}
Instruction::~Instruction() {}

CodeFragmentType Instruction::getCodeFragmentType() const { return CodeFragmentType::Instruction; }

bool Instruction::isStatement() const { return false; }

bool Instruction::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Instruction))
		return operator==(reinterpret_cast<const Instruction&>(c));
	return false;
}











Scope::Scope() :
	_insts{}
{}
Scope::Scope(const Scope& s) :
	_insts{ s._insts }
{}
Scope::Scope(Scope&& s) noexcept :
	_insts{ std::move(s._insts) }
{}
Scope::~Scope() {}

Scope& Scope::operator= (const Scope& s)
{
	_insts = s._insts;
	return *this;
}
Scope& Scope::operator= (Scope&& s) noexcept
{
	_insts = std::move(s._insts);
	return *this;
}

size_t Scope::size() const { return _insts.size(); }
bool Scope::empty() const { return _insts.empty(); }

const Instruction& Scope::getInstruction(size_t idx) const { return _insts[idx]; }

const CloneableVector<Instruction>& Scope::getAllInstructions() const { return _insts; }

void Scope::addInstruction(const Instruction& inst) { _insts.push_back(inst); }

CodeFragmentType Scope::getCodeFragmentType() const { return CodeFragmentType::Scope; }

bool Scope::isStatement() const { return false; }

static std::string Scope_toString(const CloneableVector<Instruction>& insts, size_t identation)
{
	if (insts.empty())
		return ident(identation) + "{}";

	std::stringstream ss;
	ss << ident(identation) << "{" << std::endl;
	for (const auto& inst : insts)
		ss << inst->toString(identation + 4) << std::endl;
	ss << ident(identation) << "}";

	return ss.str();
}

std::string Scope::toString(size_t identation) const { return Scope_toString(_insts, identation); }

void* Scope::clone() const { return new Scope{ *this }; }

bool Scope::operator== (const CodeFragment& c) const
{
	if (c.is(CodeFragmentType::Scope))
		return operator==(reinterpret_cast<const Scope&>(c));
	return false;
}
bool Scope::operator== (const Scope& s) const { return _insts == s._insts; }
bool Scope::operator!= (const Scope& s) const { return _insts != s._insts; }

const Instruction& Scope::operator[] (size_t idx) const { return _insts[idx]; }











InstructionStatement::InstructionStatement() :
	Instruction{},
	_statement{}
{}
InstructionStatement::InstructionStatement(const Statement& statement) :
	Instruction{},
	_statement{ statement }
{}
InstructionStatement::InstructionStatement(const InstructionStatement& inst) :
	Instruction{ inst },
	_statement{ inst._statement }
{}
InstructionStatement::InstructionStatement(InstructionStatement&& inst) noexcept :
	Instruction{ std::move(inst) },
	_statement{ std::move(inst._statement) }
{}
InstructionStatement::~InstructionStatement() {}

InstructionStatement& InstructionStatement::operator= (const InstructionStatement& inst)
{
	_statement = inst._statement;
	return *this;
}
InstructionStatement& InstructionStatement::operator= (InstructionStatement&& inst) noexcept
{
	_statement = std::move(inst._statement);
	return *this;
}

bool InstructionStatement::empty() const { return !_statement; }

const Statement& InstructionStatement::getStatement() const { return _statement; }

Instruction::Type InstructionStatement::getInstructionType() const { return Type::Statement; }

std::string InstructionStatement::toString(size_t identation) const
{
	if (!_statement)
		return ident(identation) + ";";
	return ident(identation) + _statement->toString() + ";";
}

void* InstructionStatement::clone() const { return new InstructionStatement{ *this }; }

bool InstructionStatement::operator== (const Instruction& inst) const
{
	if (inst.getInstructionType() == Type::Statement)
		return operator==(reinterpret_cast<const InstructionStatement&>(inst));
	return false;
}
bool InstructionStatement::operator== (const InstructionStatement& inst) const { return _statement == inst._statement; }
bool InstructionStatement::operator!= (const InstructionStatement& inst) const { return _statement != inst._statement; }











InstructionStatementScope::InstructionStatementScope() :
	Instruction{},
	_insts{}
{}
InstructionStatementScope::InstructionStatementScope(const Scope& scope) :
	Instruction{},
	_insts{ scope.getAllInstructions() }
{}
InstructionStatementScope::InstructionStatementScope(const InstructionStatementScope& inst) :
	Instruction{ inst },
	_insts{ inst._insts }
{}
InstructionStatementScope::InstructionStatementScope(InstructionStatementScope&& inst) noexcept :
	Instruction{ std::move(inst) },
	_insts{ std::move(inst._insts) }
{}
InstructionStatementScope::~InstructionStatementScope() {}

InstructionStatementScope& InstructionStatementScope::operator= (const InstructionStatementScope& inst)
{
	_insts = inst._insts;
	return *this;
}
InstructionStatementScope& InstructionStatementScope::operator= (InstructionStatementScope&& inst) noexcept
{
	_insts = std::move(inst._insts);
	return *this;
}

bool InstructionStatementScope::empty() const { return _insts.empty(); }
size_t InstructionStatementScope::size() const { return _insts.size(); }

const Instruction& InstructionStatementScope::getInstruction(size_t idx) const { return _insts[idx]; }

Instruction::Type InstructionStatementScope::getInstructionType() const { return Type::StatementScope; }

std::string InstructionStatementScope::toString(size_t identation) const { return ident(identation) + Scope_toString(_insts, identation); }

void* InstructionStatementScope::clone() const { return new InstructionStatementScope{ *this }; }

bool InstructionStatementScope::operator== (const Instruction& inst) const
{
	if (inst.getInstructionType() == Type::StatementScope)
		return operator==(reinterpret_cast<const InstructionStatementScope&>(inst));
	return false;
}
bool InstructionStatementScope::operator== (const InstructionStatementScope& inst) const { return _insts == inst._insts; }
bool InstructionStatementScope::operator!= (const InstructionStatementScope& inst) const { return _insts != inst._insts; }

const Instruction& InstructionStatementScope::operator[] (size_t idx) const { return _insts[idx]; }










InstructionVarDeclaration::Entry::Entry() :
	_id{ "" },
	_init{}
{}
InstructionVarDeclaration::Entry::Entry(const Identifier& identifier) :
	_id{ identifier },
	_init{}
{}
InstructionVarDeclaration::Entry::Entry(const Identifier& identifier, const Statement& initValue) :
	_id{ identifier },
	_init{ initValue }
{}

const Identifier& InstructionVarDeclaration::Entry::getIdentifier() const { return _id; }

bool InstructionVarDeclaration::Entry::hasInitValue() const { return _init; }
const Statement& InstructionVarDeclaration::Entry::getInitValue() const { return _init; }

bool InstructionVarDeclaration::Entry::operator== (const InstructionVarDeclaration::Entry& e) const { return _id == e._id && _init == e._init; }
bool InstructionVarDeclaration::Entry::operator!= (const InstructionVarDeclaration::Entry& e) const { return _id != e._id || _init != e._init; }



InstructionVarDeclaration::InstructionVarDeclaration() :
	_entries{}
{}
InstructionVarDeclaration::InstructionVarDeclaration(const std::vector<Entry>& entries) :
	_entries{ entries }
{}
InstructionVarDeclaration::InstructionVarDeclaration(const InstructionVarDeclaration& inst) :
	_entries{ inst._entries }
{}
InstructionVarDeclaration::InstructionVarDeclaration(InstructionVarDeclaration&& inst) noexcept :
	_entries{ std::move(inst._entries) }
{}
InstructionVarDeclaration::~InstructionVarDeclaration() {}

InstructionVarDeclaration& InstructionVarDeclaration::operator= (const InstructionVarDeclaration& inst)
{
	_entries = inst._entries;
	return *this;
}
InstructionVarDeclaration& InstructionVarDeclaration::operator= (InstructionVarDeclaration&& inst) noexcept
{
	_entries = std::move(inst._entries);
	return *this;
}

bool InstructionVarDeclaration::empty() const { return _entries.empty(); }
size_t InstructionVarDeclaration::size() const { return _entries.size(); }

const InstructionVarDeclaration::Entry& InstructionVarDeclaration::getEntry(size_t idx) const { return _entries[idx]; }

Instruction::Type InstructionVarDeclaration::getInstructionType() const { return Type::VarDeclaration; }

std::string InstructionVarDeclaration::toString(size_t identation) const
{
	if (_entries.empty())
		return ident(identation) + ";";

	std::stringstream ss;
	bool first = true;
	for (const Entry& e : _entries)
	{
		if (!first)
			ss << ", ";
		else first = false;

		ss << e.getIdentifier();
		if (e.hasInitValue())
			ss << " = " << e.getInitValue();
	}
	ss << ";";

	return ss.str();
}

void* InstructionVarDeclaration::clone() const { return new InstructionVarDeclaration{ *this }; }

bool InstructionVarDeclaration::operator== (const Instruction& inst) const
{
	if (inst.getInstructionType() == Type::VarDeclaration)
		return operator==(reinterpret_cast<const InstructionVarDeclaration&>(inst));
	return false;
}
bool InstructionVarDeclaration::operator== (const InstructionVarDeclaration& inst) const { return _entries == inst._entries; }
bool InstructionVarDeclaration::operator!= (const InstructionVarDeclaration& inst) const { return _entries != inst._entries; }

const InstructionVarDeclaration::Entry& InstructionVarDeclaration::operator[] (size_t idx) const { return _entries[idx]; }
