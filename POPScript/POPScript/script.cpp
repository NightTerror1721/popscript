#include "script.h"

#include <string>
#include <sstream>
#include <fstream>



Script::Script() :
	_data{}
{
	clear();
}


void Script::setCode(const size_t index, const CodeValue code)
{
	if(index >= MAX_CODES)
		throw BadIndex{ index, 0, MAX_CODES };
	_codes[index] = code;
}
CodeValue Script::getCode(const size_t index) const
{
	if (index >= MAX_CODES)
		throw BadIndex{ index, 0, MAX_CODES };
	return _codes[index];
}

ScriptCodeAccesor Script::codes() { return _codes; }
const ScriptCodeAccesor Script::codes() const { return _codes; }


void Script::setField(const size_t index, const ScriptField& field)
{
	if (index >= MAX_FIELDS)
		throw BadIndex{ index, 0, MAX_FIELDS };
	_fields[index] = field;
}
const ScriptField& Script::getField(const size_t index) const
{
	if (index >= MAX_FIELDS)
		throw BadIndex{ index, 0, MAX_FIELDS };
	return _fields[index];
}

ScriptFieldAccesor Script::fields() { return _fields; }
const ScriptFieldAccesor Script::fields() const { return _fields; }


void Script::setVersion()
{
	_data[0] = SCRIPT_VERSION;
	_data[1] = 0U;
}
uint16_t Script::getVersion() const { return static_cast<uint16_t>(*_data); }


void Script::clear()
{
	std::memset(_codes, 0, sizeof(_codes));
	wide_memset(_fields, ScriptField::invalid(), MAX_FIELDS);
}

void Script::clearCodes() { std::memset(_codes, 0, sizeof(_codes)); }
void Script::clearFields() { wide_memset(_fields, ScriptField::invalid(), MAX_FIELDS); }


void Script::read(std::istream& is)
{
	clear();

	if (is && !is.eof())
		is.read(reinterpret_cast<char*>(_codes), sizeof(_codes));
	if (is && !is.eof())
		is.read(reinterpret_cast<char*>(_fields), sizeof(_fields));
}
void Script::write(std::ostream& os) const
{
	if (os)
		os.write(reinterpret_cast<const char*>(_data), sizeof(_data));
}

void Script::readFromFile(const std::string& file)
{
	std::fstream f{ file, std::fstream::in | std::fstream::binary };
	read(f);
	f.close();
}
void Script::writeToFile(const std::string& file) const
{
	std::fstream f{ file, std::fstream::out | std::fstream::binary };
	write(f);
	f.close();
}


std::istream& operator>> (std::istream& is, Script& s)
{
	s.read(is);
	return is;
}
std::ostream& operator<< (std::ostream& os, const Script& s)
{
	s.write(os);
	return os;
}







ScriptCodeBuilder::ScriptCodeBuilder() :
	_front{ nullptr },
	_back{ nullptr },
	_size{ 0 }
{}

ScriptCodeBuilder::~ScriptCodeBuilder()
{
	clear();
}

void ScriptCodeBuilder::clear()
{
	Node* node = _front;
	while (node)
	{
		Node* const next = node->next;
		delete node;
		node = next;
	}
	_front = nullptr;
	_back = nullptr;
	_size = 0;
}

CodeLocation ScriptCodeBuilder::push_back(const CodeValue code)
{
	if (_size >= MAX_CODES)
		throw FullCodeData{};

	if (!_front)
	{
		_front = new Node{ this, code, nullptr, nullptr };
		_back = _front;
		++_size;
		return _front;
	}

	Node* const node{ new Node{ this, code, nullptr, _back } };
	_back->next = node;
	_back = node;
	++_size;
	return node;
}
CodeLocation ScriptCodeBuilder::push_front(const CodeValue code)
{
	if (_size >= MAX_CODES)
		throw FullCodeData{};

	if (!_front)
	{
		_front = new Node{ this, code, nullptr, nullptr };
		_back = _front;
		++_size;
		return _front;
	}

	Node* const node{ new Node{ this, code, _front, nullptr } };
	_front->prev = node;
	_front = node;
	++_size;
	return node;
}

CodeValue& ScriptCodeBuilder::front() { return _front->code; }
const CodeValue& ScriptCodeBuilder::front() const { return _front->code; }

CodeValue& ScriptCodeBuilder::back() { return _back->code; }
const CodeValue& ScriptCodeBuilder::back() const { return _back->code; }

CodeLocation ScriptCodeBuilder::insert_before(const CodeLocation location, const CodeValue code)
{
	if (location->builder != this)
		throw INVALID_PARAMETER(location->builder);
	if (_size >= MAX_CODES)
		throw FullCodeData{};

	Node* const base = const_cast<Node*>(location);
	if (!base->prev)
		return push_front(code);

	Node* node{ new Node{ this, code, base, base->prev } };
	base->prev->next = node;
	base->prev = node;
	++_size;
	return node;
}
CodeLocation ScriptCodeBuilder::insert_after(const CodeLocation location, const CodeValue code)
{
	if (location->builder != this)
		throw INVALID_PARAMETER(location->builder);
	if (_size >= MAX_CODES)
		throw FullCodeData{};

	Node* const base = const_cast<Node*>(location);
	if (!base->next)
		return push_back(code);

	Node* node{ new Node{ this, code, base->next, base } };
	base->next->prev = node;
	base->next = node;
	++_size;
	return node;
}

uint16_t ScriptCodeBuilder::size() const { return _size; }
bool ScriptCodeBuilder::empty() const { return _size <= 0; }

void ScriptCodeBuilder::build(Script& script) const
{
	script.clearCodes();
	ScriptCodeAccesor accesor = script.codes();
	size_t count = 0;
	for (Node* node = _front; node && count < ScriptCodeAccesor::size(); node = node->next)
		accesor[count++] = node->code;
}
