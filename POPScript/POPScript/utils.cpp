#include "utils.h"

#include <string>
#include <sstream>


static std::string BadIndex_GenerateMsg(int index, int min, int max, const char* msg)
{
	std::stringstream ss;
	ss << "Require index between " << min << " and " << max << ". But found " << index << ". " << msg;
	return ss.str();
}

static std::string InvalidParameter_GenerateMsg(const char* parameter, const char* msg)
{
	std::stringstream ss;
	ss << "Invalid value for " << parameter << " parameter. " << msg;
	return ss.str();
}

static std::string ParserError_GenerateMsg(const size_t line, const char* msg)
{
	std::stringstream ss;
	ss << "A parsing error ocurried in " << line << " line. " << msg;
	return ss.str();
}



BadIndex::BadIndex(int index, int min, int max, const char* msg) :
	exception{ BadIndex_GenerateMsg(index, min, max, msg).c_str() }
{}

IllegalState::IllegalState(const char* msg) :
	exception{ msg }
{}

InvalidParameter::InvalidParameter(const char* parameter, const char* msg) :
	exception{ InvalidParameter_GenerateMsg(parameter, msg).c_str() }
{}

UnexpectedNull::UnexpectedNull(const char* msg) :
	exception{ msg }
{}

ParserError::ParserError(const size_t line, const char* msg) :
	exception{ ParserError_GenerateMsg(line, msg).c_str() },
	_line{ line }
{}

size_t ParserError::line() const { return _line; }







ErrorList::Entry::Entry(size_t start, size_t end, const std::string& msg) :
	_startLine{ start },
	_endLine{ end },
	_msg{ msg }
{}

size_t ErrorList::Entry::startLine() const { return _startLine; }
size_t ErrorList::Entry::endLine() const { return _endLine; }

const std::string& ErrorList::Entry::message() const { return _msg; }


size_t ErrorList::size() const { return _errors.size(); }
bool ErrorList::empty() const { return _errors.empty(); }

const ErrorList::Entry& ErrorList::operator[] (const size_t index) const { return _errors[index]; }

void ErrorList::add(size_t startLine, size_t endLine, const std::string& msg) { _errors.emplace_back(startLine, endLine, msg); }

ErrorList& operator<< (ErrorList& list, const ParserError& error) { list.add(error.line(), error.line(), error.what()); return list; }
