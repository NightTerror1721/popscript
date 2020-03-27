#pragma once

#include <type_traits>
#include <regex>

#include "types.h"
#include "utils.h"
#include "functions.h"

enum class CodeFragmentType
{
	Identifier,
	LiteralInteger,
	TypeConstant,
	Stopchar,
	FunctionArguments,
	Operator,
	Operation,
	FunctionCall,
	CommandArguments,
	Command,
	Instruction,
	Scope,
};



class CodeFragment : public Cloneable
{
public:
	virtual ~CodeFragment() {}

	virtual CodeFragmentType getCodeFragmentType() const = 0;

	virtual bool isStatement() const = 0;

	virtual std::string toString(size_t identation = 0) const = 0;

	virtual void* clone() const override = 0;

	virtual bool operator== (const CodeFragment& c) const = 0;


	inline bool operator!= (const CodeFragment& c) const { return !operator==(c); }


	bool is(CodeFragmentType type) const;

	template<typename... _Args>
	bool is(CodeFragmentType t0, _Args&&... args) const
	{
		static_assert(std::is_convertible<_Args, CodeFragmentType>::value);

		auto type = getCodeFragmentType();
		if (type == t0)
			return true;

		std::vector<CodeFragmentType> vargs = { static_cast<CodeFragmentType&&>(args)... };
		for (auto t : vargs)
			if (type == t)
				return true;
		return false;
	}
};

std::ostream& operator<< (std::ostream& os, const CodeFragment& cf);



class Statement : public CodeFragment
{
public:
	virtual ~Statement() {}

	bool isStatement() const override;
};



class Identifier : public Statement
{
public:
	class InvalidIdentifier : public std::exception {};

private:
	std::string _id;

public:
	Identifier(const std::string& identifier);
	Identifier(const Identifier& id);
	Identifier(Identifier&& id) noexcept;
	~Identifier();

	Identifier& operator= (const Identifier& id);
	Identifier& operator= (Identifier&& id) noexcept;

	CodeFragmentType getCodeFragmentType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const Identifier& id) const;
	bool operator!= (const Identifier& id) const;

private:
	static const std::regex _pattern;

public:
	static bool isValid(const std::string& str);
};



class LiteralInteger : public Statement
{
private:
	FieldValue _value;

public:
	LiteralInteger(FieldValue value);
	LiteralInteger(const LiteralInteger& lit);
	LiteralInteger(LiteralInteger&& lit) noexcept;
	~LiteralInteger();

	LiteralInteger& operator= (const LiteralInteger& lit);
	LiteralInteger& operator= (LiteralInteger&& lit) noexcept;

	FieldValue getValue() const;

	CodeFragmentType getCodeFragmentType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const LiteralInteger& lit) const;
	bool operator!= (const LiteralInteger& lit) const;

private:
	static const std::regex _pattern;

public:
	static LiteralInteger parse(const std::string& str);
	static bool isValid(const std::string& str);
};



class TypeConstant : public Statement
{
private:
	DataType _type;
	ScriptCode _value;

public:
	TypeConstant(ScriptCode value);
	TypeConstant(const TypeConstant& tc);
	TypeConstant(TypeConstant&& tc) noexcept;
	~TypeConstant();

	TypeConstant& operator= (const TypeConstant& tc);
	TypeConstant& operator= (TypeConstant&& tc) noexcept;

	bool isValid() const;

	DataType getType() const;
	ScriptCode getValue() const;

	CodeFragmentType getCodeFragmentType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const TypeConstant& tc) const;
	bool operator!= (const TypeConstant& tc) const;


	static bool isValid(const std::string& str);
	static bool isValid(ScriptCode code);
};



class Stopchar : public CodeFragment
{
private:
	char _symbol;

public:
	Stopchar(const Stopchar& sc);
	~Stopchar();

	Stopchar& operator= (const Stopchar& sc);

	CodeFragmentType getCodeFragmentType() const override;

	bool isStatement() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const Stopchar& sc) const;
	bool operator!= (const Stopchar& sc) const;

private:
	Stopchar(char symbol);

public:
	static const Stopchar Semicolon;
	static const Stopchar Comma;
	static const Stopchar TwoPoints;
};



namespace
{
	class ArgumentList
	{
	private:
		CloneableVector<Statement> _args;

	public:
		ArgumentList();
		ArgumentList(const ArgumentList& a);
		ArgumentList(ArgumentList&& a) noexcept;
		virtual ~ArgumentList();

		ArgumentList& operator= (const ArgumentList& a);
		ArgumentList& operator= (ArgumentList&& a) noexcept;

		bool empty() const;
		size_t size() const;

		void addArgument(const Statement& arg);

		const Statement& operator[] (const size_t idx) const;

		std::string toString() const;

		bool operator== (const ArgumentList& other) const;
		bool operator!= (const ArgumentList& other) const;
	};
}



class FunctionArguments : public Statement, public ArgumentList
{
public:
	FunctionArguments();
	FunctionArguments(const FunctionArguments& a);
	FunctionArguments(FunctionArguments&& a) noexcept;
	~FunctionArguments();

	FunctionArguments& operator= (const FunctionArguments& a);
	FunctionArguments& operator= (FunctionArguments&& a) noexcept;

	CodeFragmentType getCodeFragmentType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const FunctionArguments& a) const;
	bool operator!= (const FunctionArguments& a) const;
};



class CommandArguments : public CodeFragment, public ArgumentList
{
public:
	CommandArguments();
	CommandArguments(const CommandArguments& a);
	CommandArguments(CommandArguments&& a) noexcept;
	~CommandArguments();

	CommandArguments& operator= (const CommandArguments& a);
	CommandArguments& operator= (CommandArguments&& a) noexcept;

	CodeFragmentType getCodeFragmentType() const override;

	bool isStatement() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const CommandArguments& a) const;
	bool operator!= (const CommandArguments& a) const;
};



class Operator : public CodeFragment
{
public:
	enum class Type
	{
		Unary,
		Binary,
		Ternary,
		Assignment
	};

private:
	uint8_t _id;
	std::string _symbol;
	Type _type;
	unsigned int _priority;
	bool _rightToLeft;
	bool _conditional;

	Operator(const std::string& symbol, const Type type, unsigned int priority, bool rightToLeft, bool conditional);

public:
	Operator(const Operator& op);
	Operator(Operator&& op) noexcept;
	~Operator();

	Operator& operator= (const Operator& op);
	Operator& operator= (Operator&& op) noexcept;

	unsigned int getPriority() const;
	bool hasRightToLeft() const;
	bool isConditional() const;

	bool isUnary() const;
	bool isBinary() const;
	bool isTernary() const;
	bool isAssignment() const;

	int comparePriority(const Operator& other) const;

	CodeFragmentType getCodeFragmentType() const override;

	bool isStatement() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const Operator& op) const;
	bool operator!= (const Operator& op) const;

public:
	static const Operator SufixIncrement;
	static const Operator SufixDecrement;

	static const Operator PrefixIncrement;
	static const Operator PrefixDecrement;
	static const Operator UnaryMinus;
	static const Operator BinaryNot;

	static const Operator Multiplication;
	static const Operator Division;

	static const Operator Addition;
	static const Operator Subtraction;

	static const Operator GreaterThan;
	static const Operator SmallerThan;
	static const Operator GreaterEqualsThan;
	static const Operator SmallerEqualsThan;

	static const Operator EqualsTo;
	static const Operator NotEqualsTo;

	static const Operator BinaryAnd;
	static const Operator BinaryOr;

	static const Operator TernaryConditional;

	static const Operator Assignment;
	static const Operator AssignmentAddition;
	static const Operator AssignmentSubtraction;
	static const Operator AssignmentMultiplication;
	static const Operator AssignmentDivision;
};



class Operation : public Statement
{
public:
	class BadOperation : public std::exception
	{
	public:
		BadOperation(const char* msg = "");
	};

private:
	Operator _operator;
	CloneableAllocator<Statement> _operands[3];

public:
	Operation(const Operation& o);
	Operation(Operation&& o) noexcept;
	~Operation();

	Operation& operator= (const Operation& o);
	Operation& operator= (Operation&& o) noexcept;

	bool isUnary() const;
	bool isBinary() const;
	bool isTernary() const;
	bool isAssignment() const;

	const Operator& getOperator() const;

	size_t getOperandCount() const;
	const Statement& getOperand(const size_t idx) const;

	CodeFragmentType getCodeFragmentType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const Operation& o) const;
	bool operator!= (const Operation& o) const;

	inline const Statement& unaryOperand() { return _operands[0]; }

	inline const Statement& leftOperand() { return _operands[0]; }
	inline const Statement& rightOperand() { return _operands[1]; }

	inline const Statement& condOperand() { return _operands[0]; }
	inline const Statement& trueCaseOperand() { return _operands[1]; }
	inline const Statement& falseCaseOperand() { return _operands[2]; }

private:
	Operation(const Operator& op, const Statement& operand0, const Statement* operand1 = nullptr, const Statement* operand2 = nullptr);

public:
	static Operation unary(const Operator& op, const Statement& operand);
	static Operation binary(const Operator& op, const Statement& left, const Statement& right);
	static Operation ternary(const Statement& cond, const Statement& opIfTrue, const Statement& opIfFalse);
	static Operation assignment(const Operator& op, const Statement& left, const Statement& right);
};



class FunctionCall : public Statement
{
public:
	class BadFunctionCall : public std::exception
	{
	public:
		BadFunctionCall(const char* msg = "");
	};

private:
	const Callable* _callable;
	FunctionArguments _args;

public:
	FunctionCall();
	FunctionCall(const FunctionCall& fc);
	FunctionCall(FunctionCall&& fc) noexcept;
	~FunctionCall();

	FunctionCall& operator= (const FunctionCall& fc);
	FunctionCall& operator= (FunctionCall&& fc) noexcept;

	const Callable& getCallable() const;
	const FunctionArguments& getArguments() const;

	CodeFragmentType getCodeFragmentType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const FunctionCall& fc) const;
	bool operator!= (const FunctionCall& fc) const;

private:
	FunctionCall(const Callable& callable, const FunctionArguments& args);

public:
	static FunctionCall make(const Callable& callable, const Statement& args);
};



class Command : public CodeFragment
{
private:
	uint8_t _id;
	std::string _name;

public:
	Command();
	Command(const Command& c);
	Command(Command&& c) noexcept;
	~Command();

	Command& operator= (const Command& c);
	Command& operator= (Command&& c) noexcept;

	CodeFragmentType getCodeFragmentType() const override;

	bool isStatement() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const Command& c) const;
	bool operator!= (const Command& c) const;

private:
	Command(const std::string& name);

public:
	static const Command Invalid;
	static const Command Var;
	static const Command Const;
	static const Command Define;
	static const Command Import;
	static const Command If;
	static const Command Else;
	static const Command Every;
};



class Instruction : public CodeFragment
{
public:
	enum class Type
	{
		Statement,
		StatementScope,
		VarDeclaration,
		ConstDeclaration,
		//DefineDeclaration,
		//Import,
		Conditional,
		EveryLoop
	};

public:
	Instruction();
	virtual ~Instruction();

	CodeFragmentType getCodeFragmentType() const override;

	bool isStatement() const override;

	virtual Type getInstructionType() const = 0;

	bool operator== (const CodeFragment& c) const override;

	virtual bool operator== (const Instruction& c) const = 0;
	inline bool operator!= (const Instruction& c) const { return !operator==(c); }
};



class Scope : public CodeFragment
{
private:
	CloneableVector<Instruction> _insts;

public:
	Scope();
	Scope(const Scope& s);
	Scope(Scope&& s) noexcept;
	~Scope();

	Scope& operator= (const Scope& s);
	Scope& operator= (Scope&& s) noexcept;

	size_t size() const;
	bool empty() const;

	const Instruction& getInstruction(size_t idx) const;

	const CloneableVector<Instruction>& getAllInstructions() const;

	void addInstruction(const Instruction& inst);

	CodeFragmentType getCodeFragmentType() const override;

	bool isStatement() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const CodeFragment& c) const override;
	bool operator== (const Scope& s) const;
	bool operator!= (const Scope& s) const;

	const Instruction& operator[] (size_t idx) const;
};



class InstructionStatement : public Instruction
{
private:
	CloneableAllocator<Statement> _statement;

public:
	InstructionStatement();
	InstructionStatement(const Statement& statement);
	InstructionStatement(const InstructionStatement& inst);
	InstructionStatement(InstructionStatement&& inst) noexcept;
	~InstructionStatement();

	InstructionStatement& operator= (const InstructionStatement& inst);
	InstructionStatement& operator= (InstructionStatement&& inst) noexcept;

	bool empty() const;

	const Statement& getStatement() const;

	Instruction::Type getInstructionType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const Instruction& inst) const override;
	bool operator== (const InstructionStatement& inst) const;
	bool operator!= (const InstructionStatement& inst) const;
};



class InstructionStatementScope : public Instruction
{
private:
	CloneableVector<Instruction> _insts;

public:
	InstructionStatementScope();
	InstructionStatementScope(const Scope& scope);
	InstructionStatementScope(const InstructionStatementScope& inst);
	InstructionStatementScope(InstructionStatementScope&& inst) noexcept;
	~InstructionStatementScope();

	InstructionStatementScope& operator= (const InstructionStatementScope& inst);
	InstructionStatementScope& operator= (InstructionStatementScope&& inst) noexcept;

	bool empty() const;
	size_t size() const;

	const Instruction& getInstruction(size_t idx) const;

	Instruction::Type getInstructionType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const Instruction& inst) const override;
	bool operator== (const InstructionStatementScope& inst) const;
	bool operator!= (const InstructionStatementScope& inst) const;

	const Instruction& operator[] (size_t idx) const;
};



class InstructionVarDeclaration : public Instruction
{
public:
	class Entry
	{
	private:
		Identifier _id;
		CloneableAllocator<Statement> _init;

	public:
		Entry();
		Entry(const Identifier& identifier);
		Entry(const Identifier& identifier, const Statement& initValue);
		Entry(const Entry&) = default;
		Entry(Entry&&) noexcept = default;

		Entry& operator= (const Entry& e) = default;
		Entry& operator= (Entry&& e) noexcept = default;

		const Identifier& getIdentifier() const;

		bool hasInitValue() const;
		const Statement& getInitValue() const;

		bool operator== (const InstructionVarDeclaration::Entry& e) const;
		bool operator!= (const InstructionVarDeclaration::Entry& e) const;
	};

private:
	std::vector<Entry> _entries;

public:
	InstructionVarDeclaration();
	InstructionVarDeclaration(const std::vector<Entry>& entries);
	InstructionVarDeclaration(const InstructionVarDeclaration& inst);
	InstructionVarDeclaration(InstructionVarDeclaration&& inst) noexcept;
	~InstructionVarDeclaration();

	InstructionVarDeclaration& operator= (const InstructionVarDeclaration& inst);
	InstructionVarDeclaration& operator= (InstructionVarDeclaration&& inst) noexcept;

	bool empty() const;
	size_t size() const;

	const Entry& getEntry(size_t idx) const;

	Instruction::Type getInstructionType() const override;

	std::string toString(size_t identation = 0) const override;

	void* clone() const override;

	bool operator== (const Instruction& inst) const override;
	bool operator== (const InstructionVarDeclaration& inst) const;
	bool operator!= (const InstructionVarDeclaration& inst) const;

	const Entry& operator[] (size_t idx) const;
};
