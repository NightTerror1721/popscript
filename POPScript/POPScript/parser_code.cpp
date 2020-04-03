#include "parser.h"

#include <sstream>

namespace parser
{
	CodeParser::Queue::Queue(const CodeFragment& last) :
		_q{},
		_last{ last }
	{}

	size_t CodeParser::Queue::size() const { return _q.size(); }
	bool CodeParser::Queue::empty() const { return _q.empty(); }

	const CodeFragment& CodeParser::Queue::front() const { return _q.front(); }

	void CodeParser::Queue::push(const CodeFragment& frag) { _q.emplace_back(frag); }

	bool CodeParser::Queue::pop()
	{
		if (!_q.empty())
		{
			_q.pop_front();
			return true;
		}
		return false;
	}

	void CodeParser::Queue::setLast(const CodeFragment& frag) { _last = frag; }
	void CodeParser::Queue::eraseLast() { _last = nullptr; }
	bool CodeParser::Queue::hasLast() const { return _last; }
	const CodeFragment& CodeParser::Queue::getLast() const { return _last; }

	void CodeParser::Queue::push_ret(const CodeFragment& frag, CloneableAllocator<CodeFragment>& dst)
	{
		if (_q.empty())
			dst = frag;
		else
		{
			dst = _q.front();
			_q.pop_front();
			_q.emplace_back(frag);
		}
	}
}

parser::CodeParser::Queue& operator<< (parser::CodeParser::Queue& q, const CodeFragment& frag)
{
	q.push(frag);
	return q;
}
parser::CodeParser::Queue& operator>> (parser::CodeParser::Queue& q, CloneableAllocator<CodeFragment>& frag)
{
	if (q)
	{
		frag = q.front();
		q.pop();
	}
	return q;
}










namespace parser
{
	CodeParser::Builder::Builder(Queue& queue, bool canFinish) :
		_q{ &queue },
		_ss{},
		_canFinish{ canFinish },
		_finishedEnabled{ true }
	{}
	CodeParser::Builder::~Builder() {}

	size_t CodeParser::Builder::size() const { return _ss.str().size(); }
	bool CodeParser::Builder::empty() const { return _ss.str().empty(); }

	void CodeParser::Builder::clear() { _ss.str(""); }

	void CodeParser::Builder::enableFinish() { _finishedEnabled = true; }
	void CodeParser::Builder::disableFinish() { _finishedEnabled = false; }

	bool CodeParser::Builder::canFinish() const { return  _canFinish && _finishedEnabled; }

	bool CodeParser::Builder::flush()
	{
		if (empty())
			return !_q->empty();
		CloneableAllocator<CodeFragment> frag = decode();
		clear();
		_q->push(frag);
		return true;
	}

	CloneableAllocator<CodeFragment> CodeParser::Builder::decode()
	{
		if (empty())
			throw IllegalState{};


		std::string text = _ss.str();
		if (text == "var") return Command::Var;
		if (text == "const") return Command::Const;
		if (text == "if") return Command::If;
		if (text == "else") return Command::Else;
		if (text == "every") return Command::Every;
		
		if (TypeConstant::isValid(text))
			return TypeConstant::parse(text);

		if (LiteralInteger::isValid(text))
			return LiteralInteger::parse(text);

		return Identifier{ text };
	}
}
