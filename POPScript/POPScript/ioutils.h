#pragma once

#include <string>
#include <vector>
#include <exception>
#include <cstdint>

namespace
{
	class Line
	{
	private:
		size_t _num;
		size_t _offset;
		size_t _len;
		size_t _endset;

		std::string _chars;

	public:
		Line(); 
		Line(size_t num, size_t offset, const std::string& buffer);
		~Line();

		size_t lineNumber() const;
		size_t offset() const;
		size_t endset() const;

		size_t size() const;

		const std::string& chars() const;

		char operator[] (const size_t idx) const;
	};
}



/*class CodeWriter
{
private:
	std::vector<std::string> _lines;
	std::stringstream _cline;

public:
	CodeWriter();
	CodeWriter(const CodeWriter&) = default;
	CodeWriter(CodeWriter&&) = default;
	~CodeWriter();

	CodeWriter& operator= (const CodeWriter&) = default;
	CodeWriter& operator= (CodeWriter&&) = default;

	friend CodeWriter& operator<< (CodeWriter& cw, const char value);
	friend CodeWriter& operator<< (CodeWriter& cw, const std::string& value);

	friend CodeWriter& operator<< (CodeWriter& cw, const uint8_t value);
	friend CodeWriter& operator<< (CodeWriter& cw, const uint16_t value);
	friend CodeWriter& operator<< (CodeWriter& cw, const uint32_t value);
	friend CodeWriter& operator<< (CodeWriter& cw, const uint64_t value);

	friend CodeWriter& operator<< (CodeWriter& cw, const int8_t value);
	friend CodeWriter& operator<< (CodeWriter& cw, const int16_t value);
	friend CodeWriter& operator<< (CodeWriter& cw, const int32_t value);
	friend CodeWriter& operator<< (CodeWriter& cw, const int64_t value);

	friend CodeWriter& operator<< (CodeWriter& cw, const bool value);
};*/



class EOFException : std::exception {};

class CodeReader
{
private:
	std::vector<Line> _source;
	size_t _index;
	size_t _size;
	size_t _start;
	const Line* _lcur;

public:
	CodeReader();
	CodeReader(const CodeReader& cr);
	CodeReader(CodeReader&& cr) noexcept = default;
	~CodeReader();

	CodeReader& operator= (const CodeReader& cr);
	CodeReader& operator= (CodeReader&& cr) = default;

	void load(std::istream& is);
	void load(const std::string& str);

	CodeReader subpart(size_t from, size_t to) const;

	size_t currentLine() const;

	void reset();

	char next();

	std::string nextString(size_t count);

	char peek() const;
	char peek(intmax_t positions) const;
	bool canPeek(intmax_t positions) const;

	void move(intmax_t positions);

	void seekOrEnd(char c);

	void seekOrEnd(char c1, char c2);

	bool hasNext() const;

	size_t getMaxIndex() const;

	size_t getCurrentIndex() const;

	char setIndex(size_t index);

	bool findIgnoreSpaces(char c) const;


private:
	size_t _moveTo(size_t to) const;
	char peekTo(size_t to) const;
	char moveTo(size_t to);

	static constexpr size_t INVALID_INDEX = static_cast<size_t>(~0LL);
};
