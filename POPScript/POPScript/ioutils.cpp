#include "ioutils.h"

#include <sstream>
#include <iostream>

#include "utils.h"


Line::Line() :
	_num{},
	_offset{},
	_len{},
	_endset{},
	_chars{}
{}
Line::Line(size_t num, size_t offset, const std::string& buffer) :
	_num{ num },
	_offset{ offset },
	_len{ buffer.size() + 1 },
	_endset{ offset + buffer.size() },
	_chars{ buffer }
{}
Line::~Line() {}

size_t Line::lineNumber() const { return _num; }
size_t Line::offset() const { return _offset; }
size_t Line::endset() const { return _endset; }

size_t Line::size() const { return _len; }

const std::string& Line::chars() const { return _chars; }

char Line::operator[] (const size_t idx) const { return idx == _len - 1 ? '\n' : _chars[idx]; }










CodeReader::CodeReader() :
	_source{},
	_index{},
	_size{},
	_start{},
	_lcur{ nullptr }
{}
CodeReader::CodeReader(const CodeReader& cr) :
	_source{ cr._source },
	_index{ cr._index },
	_size{ cr._size },
	_start{ cr._start },
	_lcur{ &_source[cr._lcur->lineNumber()] }
{}
CodeReader::~CodeReader() {}

CodeReader& CodeReader::operator= (const CodeReader& cr)
{
	_source = cr._source;
	_index = cr._index;
	_size = cr._size;
	_start = cr._start;
	_lcur = &_source[cr._lcur->lineNumber()];
	return *this;
}

#define BUFFER_LEN 8192
static std::vector<Line> readlines(std::istream& is)
{
	std::string sline;
	size_t index = 1;
	size_t size = 0;
	
	while (is)
	{
		std::getline(is, sline);
		
	}
}
#undef BUFFER_LEN

void CodeReader::load(std::istream& is)
{
	this->~CodeReader();
	new(this) CodeReader{};

	std::string sline;
	_index = 1;
	_size = 0;

	while (is)
	{
		std::getline(is, sline);
		size_t old = _size;
		_size += sline.size() + 1;
		++_index;
		_source.emplace_back(_index, old, sline);
	}

	_index = INVALID_INDEX;
	_start = 0;
	_lcur = nullptr;
}

void CodeReader::load(const std::string& str)
{
	std::istringstream iss{ str };
	load(iss);
}

CodeReader CodeReader::subpart(size_t from, size_t to) const
{
	CodeReader cr;
	cr.setIndex(from);
	cr._index = cr._start = from;
	cr._size = to;
	return std::move(cr);
}

size_t CodeReader::currentLine() const { return !_lcur || _source.empty() ? 0 : _lcur->lineNumber(); }

void CodeReader::reset()
{
	_index = INVALID_INDEX;
	_lcur = nullptr;
}

char CodeReader::next()
{
	if (_source.empty() || ( _index != INVALID_INDEX && _index >= _size))
		throw EOFException{};
	if (_index == INVALID_INDEX)
	{
		_index = 0;
		_lcur = &_source[0];
		if (_lcur->chars().empty() || _index == _lcur->chars().size())
			return '\n';
		return (*_lcur)[_index];
	}
	++_index;
	size_t offset = _index - _lcur->offset();
	if (offset == _lcur->chars().size())
		return '\n';
	if (offset > _lcur->chars().size())
	{
		if (_lcur->lineNumber() >= _source.size())
			throw EOFException{};
		_lcur = &_source[_lcur->lineNumber()];
		offset = _index - _lcur->offset();
		if (_lcur->chars().empty() || offset == _lcur->chars().size())
			return '\n';
	}
	return (*_lcur)[offset];
}

std::string CodeReader::nextString(size_t count)
{
	std::string str(count, '\0');
	for (size_t i = 0; i < count; ++i)
		str[i] = next();
	return str;
}

char CodeReader::peek() const { return peekTo(_index); }
char CodeReader::peek(intmax_t positions) const { return peekTo(static_cast<size_t>(_index + positions)); }
bool CodeReader::canPeek(intmax_t positions) const
{
	try { peekTo(static_cast<size_t>(_index + positions)); }
	catch (BadIndex ex) { return false; }

	return true;
}

size_t CodeReader::_moveTo(size_t to) const
{
	if (to < _start || to >= _size)
		throw BadIndex{ to, _start, _size };

	int line = !_lcur ? 0 : _lcur->lineNumber() - 1;
	if (to == _index)
	{
		int idx = _index - _source[line].offset();
		return idx == _source[line].chars().size()
			? '\n'
			: _source[line][idx];
	}

	if (to > _index)
	{
		while (_source[line].endset() < to)
			line++;
	}
	else
	{
		while (_source[line].offset() > to)
			line--;
	}

	return line;
}

char CodeReader::peekTo(size_t to) const
{
	size_t line = _moveTo(to);

	const Line& l = _source[line];
	int offset = to - l.offset();
	if (l.chars().empty() || offset == l.chars().size())
		return '\n';
	return l[offset];
}
char CodeReader::moveTo(size_t to)
{
	size_t line = _moveTo(to);

	_lcur = &_source[line];
	_index = to;
	int offset = _index - _lcur->offset();
	if (_lcur->chars().empty() || offset == _lcur->chars().size())
		return '\n';
	return (*_lcur)[offset];
}

void CodeReader::move(intmax_t positions)
{
	try { moveTo(static_cast<size_t>(_index + positions)); }
	catch (BadIndex ex) { throw EOFException{}; }
}

void CodeReader::seekOrEnd(char c)
{
	try { while (next() != c); }
	catch (EOFException ex) {}
}

void CodeReader::seekOrEnd(char c1, char c2)
{
	try
	{
		for (;;)
		{
			char c = next();
			if (c == c1 && canPeek(1) && peek(1) == c2)
			{
				next();
				return;
			}
		}
	}
	catch (EOFException ex) {}
}

bool CodeReader::hasNext() const { return !_source.empty() && (_index < _size || _index == INVALID_INDEX); }

size_t CodeReader::getMaxIndex() const { return _size; }

size_t CodeReader::getCurrentIndex() const { return _index; }

char CodeReader::setIndex(size_t index) { return moveTo(index); }

bool CodeReader::findIgnoreSpaces(char c) const
{
	size_t idx = _index;
	try
	{
		for (;;)
		{
			char c2 = const_cast<CodeReader*>(this)->next();
			if (c2 == ' ' || c2 == '\t')
				continue;
			if (c2 == c)
				return true;
			const_cast<CodeReader*>(this)->setIndex(idx);
			return false;
		}
	}
	catch (EOFException ex)
	{
		const_cast<CodeReader*>(this)->setIndex(idx);
		return false;
	}
}

