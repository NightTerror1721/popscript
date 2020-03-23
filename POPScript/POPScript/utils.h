#pragma once

#include <exception>

class BadIndex : std::exception
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

class InvalidParameter : std::exception
{
public:
	InvalidParameter(const char* parameter, const char* msg = "");
};

#define INVALID_PARAMETER(_Parameter) InvalidParameter{ #_Parameter }



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
