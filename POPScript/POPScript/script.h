#pragma once

#include <exception>
#include <iostream>
#include <string>

#include "consts.h"
#include "utils.h"

#define SCRIPT_VERSION 12U
#define MAX_CODES 4096U
#define MAX_FIELDS 512U
#define MAX_VARS 64U

#define CODES_ARRAY_SIZE (MAX_CODES * sizeof(CodeValue))
#define FIELDS_ARRAY_SIZE (MAX_FIELDS * sizeof(ScriptField))
#define EMPTY_DATA_ARRAY_SIZE 264
#define SCRIPT_SIZE (CODES_ARRAY_SIZE + FIELDS_ARRAY_SIZE + EMPTY_DATA_ARRAY_SIZE)


typedef Accessor<CodeValue, MAX_CODES> ScriptCodeAccesor;
typedef Accessor<ScriptField, MAX_FIELDS> ScriptFieldAccesor;


class Script
{
private:
	union
	{
		byte_t _data[SCRIPT_SIZE];
		struct
		{
			CodeValue _codes[MAX_CODES];
			ScriptField _fields[MAX_FIELDS];
			byte_t __padding[EMPTY_DATA_ARRAY_SIZE];
		};
	};

public:
	Script();


	void setCode(const size_t index, const CodeValue code);
	CodeValue getCode(const size_t index) const;

	ScriptCodeAccesor codes();
	const ScriptCodeAccesor codes() const;


	void setField(const size_t index, const ScriptField& field);
	const ScriptField& getField(const size_t index) const;

	ScriptFieldAccesor fields();
	const ScriptFieldAccesor fields() const;


	void setVersion();
	uint16_t getVersion() const;


	void clear();

	void clearCodes();
	void clearFields();


	void read(std::istream& is);
	void write(std::ostream& os) const;

	void readFromFile(const std::string& file);
	void writeToFile(const std::string& file) const;
};


std::istream& operator>> (std::istream& is, Script& s);
std::ostream& operator<< (std::ostream& os, const Script& s);






/* Script Build */

class FullCodeData : public std::exception {};


class ScriptCodeBuilder;
namespace
{
	struct ScriptCodeBuilderNode
	{
		const ScriptCodeBuilder* const builder;
		CodeValue code;
		ScriptCodeBuilderNode* next;
		ScriptCodeBuilderNode* prev;
	};
}
typedef const ScriptCodeBuilderNode* CodeLocation;


class ScriptCodeBuilder
{
private:
	typedef ScriptCodeBuilderNode Node;

	Node* _front;
	Node* _back;
	uint16_t _size;

public:
	ScriptCodeBuilder();
	~ScriptCodeBuilder();

	void clear();

	CodeLocation push_back(const CodeValue code);
	CodeLocation push_front(const CodeValue code);

	CodeValue& front();
	const CodeValue& front() const;

	CodeValue& back();
	const CodeValue& back() const;

	CodeLocation insert_before(const CodeLocation location, const CodeValue code);
	CodeLocation insert_after(const CodeLocation location, const CodeValue code);

	uint16_t size() const;
	bool empty() const;

	void build(Script& script) const;


	//constexpr ScriptCode& code(const CodeLocation location) { return const_cast<Node*>(location)->code; }
	//constexpr const ScriptCode& code(const CodeLocation location) const { return location->code; }

	constexpr CodeValue& operator[] (const CodeLocation location) { return const_cast<Node*>(location)->code; }
	constexpr const CodeValue& operator[] (const CodeLocation location) const { return location->code; }
};
