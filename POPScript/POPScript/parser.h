#pragma once

#include <list>
#include <sstream>

#include "parser_elements.h"
#include "ioutils.h"

namespace parser::statement
{
	CloneableAllocator<Statement> parse(const CodeFragmentList& list);
}

namespace parser
{
	class CodeParser
	{
	public:
		class Queue
		{
		private:
			std::list<CloneableAllocator<CodeFragment>> _q;
			CloneableAllocator<CodeFragment> _last;

		public:
			Queue() = default;
			Queue(const CodeFragment& last);
			~Queue() = default;

			size_t size() const;
			bool empty() const;

			const CodeFragment& front() const;

			void push(const CodeFragment& frag);

			bool pop();

			void setLast(const CodeFragment& frag);
			void eraseLast();
			bool hasLast() const;
			const CodeFragment& getLast() const;

			void push_ret(const CodeFragment& frag, CloneableAllocator<CodeFragment>& dst);

			inline operator bool() const { return !_q.empty(); }
			inline bool operator! () const { return _q.empty(); }
		};

		class Builder
		{
		private:
			Queue* _q;
			std::stringstream _ss;
			bool _canFinish;
			bool _finishedEnabled;

		public:
			Builder() = default;
			Builder(Queue& queue, bool canFinish);
			Builder(const Builder&) = default;
			Builder(Builder&&) = default;
			~Builder();

			Builder& operator= (const Builder&) = default;
			Builder& operator= (Builder&&) = default;

			size_t size() const;
			bool empty() const;

			void clear();

			void enableFinish();
			void disableFinish();

			bool canFinish() const;

			bool flush();

			CloneableAllocator<CodeFragment> decode();

			inline operator bool() const { return !empty(); }
			inline bool operator! () const { return empty(); }

			inline Builder& operator<< (const uint8_t value) { _ss << value; return *this; }
			inline Builder& operator<< (const uint16_t value) { _ss << value; return *this; }
			inline Builder& operator<< (const uint32_t value) { _ss << value; return *this; }
			inline Builder& operator<< (const uint64_t value) { _ss << value; return *this; }

			inline Builder& operator<< (const int8_t value) { _ss << value; return *this; }
			inline Builder& operator<< (const int16_t value) { _ss << value; return *this; }
			inline Builder& operator<< (const int32_t value) { _ss << value; return *this; }
			inline Builder& operator<< (const int64_t value) { _ss << value; return *this; }

			inline Builder& operator<< (const float value) { _ss << value; return *this; }
			inline Builder& operator<< (const double value) { _ss << value; return *this; }

			inline Builder& operator<< (const char value) { _ss << value; return *this; }

			inline Builder& operator<< (const bool value) { _ss << value; return *this; }

			inline Builder& operator<< (const std::string& value) { _ss << value; return *this; }

			inline Builder& operator<< (const CodeFragment& value) { _ss << value; return *this; }
		};
	};
}

parser::CodeParser::Queue& operator<< (parser::CodeParser::Queue& q, const CodeFragment& frag);
parser::CodeParser::Queue& operator>> (parser::CodeParser::Queue& q, CloneableAllocator<CodeFragment>& frag);
