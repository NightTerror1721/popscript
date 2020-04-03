#pragma once

#include <exception>
#include <utility>
#include <type_traits>
#include <vector>
#include <functional>
#include <algorithm>

class BadIndex : public std::exception
{
public:
	BadIndex(int index, int min, int max, const char* msg = "");

	template<typename _IndexTy, typename _MinTy, typename _MaxTy>
	inline BadIndex(_IndexTy index, _MinTy min, _MaxTy max, const char* msg = "") :
		BadIndex{
			static_cast<int>(index),
			static_cast<int>(min),
			static_cast<int>(max),
			msg
	} {}
};

class IllegalState : public std::exception
{
public:
	IllegalState(const char* msg = "");
};

class InvalidParameter : public std::exception
{
public:
	InvalidParameter(const char* parameter, const char* msg = "");
};

#define INVALID_PARAMETER(_Parameter) InvalidParameter{ #_Parameter }

class UnexpectedNull : public std::exception
{
public:
	UnexpectedNull(const char* msg = "");
};

class ParserError : public std::exception
{
private:
	size_t _line;

public:
	ParserError(const size_t line, const char* msg = "");

	size_t line() const;
};



template<typename _Ty, size_t _Size>
class Accessor
{
private:
	_Ty* _data;

public:
	Accessor(_Ty* data) : _data{ data } {}
	Accessor(const _Ty* data) : _data{ const_cast<_Ty*>(data) } {}

	_Ty& operator[] (const size_t index)
	{
		if (index >= _Size)
			throw BadIndex{ index, 0, _Size };
		return _data[index];
	}

	const _Ty& operator[] (const size_t index) const
	{
		if (index >= _Size)
			throw BadIndex{ index, 0, _Size };
		return _data[index];
	}

	static constexpr size_t size() { return _Size; }
};


template<typename _Ty>
void wide_memset(void* const _Dst, const _Ty value, const uintptr_t size)
{
	for (uintptr_t i = 0; i < (size & (~7)); ++i)
		*(reinterpret_cast<_Ty*>(_Dst) + i) = value;
}

template<typename _Ty>
std::vector<_Ty> slice(const std::vector<_Ty>& vec, size_t from, size_t to)
{
	std::vector<_Ty> res{ to - from };
	std::copy(vec.begin() + from, vec.begin() + to, res.begin());
	return std::move(res);
}





class ErrorList
{
public:
	class Entry
	{
	private:
		size_t _startLine;
		size_t _endLine;
		std::string _msg;

	public:
		Entry() = default;
		Entry(size_t start, size_t end, const std::string& msg);

		size_t startLine() const;
		size_t endLine() const;

		const std::string& message() const;
	};

	typedef typename std::vector<Entry>::iterator iterator;
	typedef typename std::vector<Entry>::const_iterator const_iterator;

private:
	std::vector<Entry> _errors;

	ErrorList() = default;
	ErrorList(const ErrorList&) = default;
	ErrorList(ErrorList&&) = default;

	ErrorList& operator= (const ErrorList&) = default;
	ErrorList& operator= (ErrorList&&) = default;

	size_t size() const;
	bool empty() const;

	const Entry& operator[] (const size_t index) const;

	void add(size_t startLine, size_t endLine, const std::string& msg);

	friend ErrorList& operator<< (ErrorList& list, const ParserError& error);

	inline operator bool() const { return !_errors.empty(); }
	inline bool operator! () const { return _errors.empty(); }

	inline iterator begin() { return _errors.begin(); }
	inline const_iterator begin() const { return _errors.begin(); }
	inline iterator end() { return _errors.end(); }
	inline const_iterator end() const { return _errors.end(); }
};









template<typename _Base>
class Conversor
{
public:
	constexpr Conversor() = default;

	template<typename _Ty>
	_Ty& as()
	{
		static_assert(std::is_base_of<_Base, _Ty>::value);
		return reinterpret_cast<_Ty&>(*this);
	}

	template<typename _Ty>
	const _Ty& as() const
	{
		static_assert(std::is_base_of<_Base, _Ty>::value);
		return reinterpret_cast<const _Ty&>(*this);
	}
};






/* Cloneable Allocator */

class Cloneable
{
public:
	virtual ~Cloneable() {};

	virtual void* clone() const = 0;
};


#define __clone(_Value) reinterpret_cast<_Base*>(reinterpret_cast<const Cloneable&>(_Value).clone())
template<class _Base>
class CloneableAllocator
{
	static_assert(std::is_base_of<Cloneable, _Base>::value);

private:
	_Base* _data;

public:
	CloneableAllocator() : _data{ nullptr } {}
	CloneableAllocator(const _Base& base) : _data{ __clone(base) } {}
	CloneableAllocator(const _Base* base) : _data{ base ? (__clone(base)) : nullptr } {}
	CloneableAllocator(const CloneableAllocator& a) : _data{ !a._data ? nullptr : __clone(a._data) } {}
	CloneableAllocator(CloneableAllocator&& a) noexcept :
		_data{ std::move(a._data) }
	{
		a._data = nullptr;
	}
	~CloneableAllocator()
	{
		if (_data)
			delete _data;
	}

	CloneableAllocator& operator= (const _Base& base)
	{
		auto old = _data;
		_data = __clone(base);
		if(old)
			delete old;
		return *this;
	}
	CloneableAllocator& operator= (decltype(nullptr))
	{
		if (_data)
			delete _data;
		_data = nullptr;
		return *this;
	}
	CloneableAllocator& operator= (const CloneableAllocator& a)
	{
		auto old = _data;
		_data = !a._data ? nullptr : __clone(a._data);
		if (old)
			delete old;
		return *this;
	}
	CloneableAllocator& operator= (CloneableAllocator&& a) noexcept
	{
		auto old = _data;
		_data = std::move(a._data);
		if (old)
			delete old;
		a._data = nullptr;
		return *this;
	}

	bool operator== (const CloneableAllocator& a) const
	{
		if (!_data)
			return !a._data;
		return a._data && _data->operator==(*a._data);
	}
	bool operator!= (const CloneableAllocator& a) const
	{
		if (!_data)
			return a._data;
		return !a._data || _data->operator!=(*a._data);
	}

	bool operator== (const _Base& b) const
	{
		if (!_data)
			return false;
		return _data->operator==(b);
	}
	bool operator!= (const _Base& b) const
	{
		if (!_data)
			return true;
		return _data->operator!=(b);
	}

	operator _Base& () { return *_data; }
	operator const _Base&() const { return *_data; }

	operator bool() const { return _data; }
	bool operator! () const { return !_data; }

	inline _Base* operator-> () { return _data; }
	inline const _Base* operator-> () const { return _data; }
};
#undef __clone


template<typename _Base>
class CloneableVector
{
	static_assert(std::is_base_of<Cloneable, _Base>::value);

public:
	typedef typename std::vector<CloneableAllocator<_Base>>::iterator iterator;
	typedef typename std::vector<CloneableAllocator<_Base>>::const_iterator const_iterator;

private:
	std::vector<CloneableAllocator<_Base>> _data;

public:
	CloneableVector() : _data{} {}
	CloneableVector(const CloneableVector& v) : _data{ v._data } {}
	CloneableVector(CloneableVector&& v) noexcept : _data{ std::move(v._data) } {}
	~CloneableVector() {}

	CloneableVector& operator= (const CloneableVector& v)
	{
		_data = v._data;
		return *this;
	}
	CloneableVector& operator= (CloneableVector&& v) noexcept
	{
		_data = std::move(v._data);
		return *this;
	}

	inline size_t size() const { return _data.size(); }
	inline bool empty() const { return _data.empty(); }

	inline const CloneableAllocator<_Base>* data() const { return _data.data(); }

	inline const std::vector<CloneableAllocator<_Base>>& stdvector() const { return _data; }

	inline void push_back(const _Base& value) { _data.emplace_back(value); }

	inline const _Base& front() const { return _data.front(); }
	inline const _Base& back() const { return _data.back(); }

	inline _Base& operator[] (const size_t index) { return _data[index]; }
	inline const _Base& operator[] (const size_t index) const { return _data[index]; }

	inline operator bool() const { return !_data.empty(); }

	inline bool operator! () { return _data.empty(); }

	bool operator== (const CloneableVector<_Base>& v) { return _data == v._data; }

	bool operator!= (const CloneableVector<_Base>& v) { return _data != v._data; }

	void for_each(std::function<void(_Base&)> action)
	{
		for (CloneableAllocator<_Base>& value : _data)
			action(value);
	}

	void for_each(std::function<void(const _Base&)> action) const
	{
		for (const CloneableAllocator<_Base>& value : _data)
			action(value);
	}

	void for_each(std::function<void(_Base&, size_t)> action)
	{
		size_t idx = 0;
		for (CloneableAllocator<_Base>& value : _data)
			action(value, idx++);
	}

	void for_each(std::function<void(const _Base&, size_t)> action) const
	{
		size_t idx = 0;
		for (const CloneableAllocator<_Base>& value : _data)
			action(value, idx++);
	}

	inline iterator begin() { return _data.begin(); }
	inline const_iterator begin() const { return _data.begin(); }
	inline const_iterator cbegin() const { return _data.cbegin(); }
	inline iterator end() { return _data.end(); }
	inline const_iterator end() const { return _data.end(); }
	inline const_iterator cend() const { return _data.cend(); }
};
