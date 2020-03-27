#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "consts.h"

namespace
{
	class _NativeDataType
	{
	private:
		uint8_t _id;
		std::string _name;

		bool _integerType;
		std::map<std::string, ScriptCode> _avByName;
		std::map<ScriptCode, std::string> _avByValue;

		ScriptCode _defvalue;

		_NativeDataType(const uint8_t id, const std::string& name);
		_NativeDataType(const uint8_t id, const std::string& name, const std::vector<std::pair<std::string, ScriptCode>>& availableValues, const std::string& defaultName, ScriptCode defaultValue);

	public:
		const std::string& name() const;

		std::vector<std::string> availableValues() const;

		bool isValidIdentifier(const std::string& identifier) const;
		bool isValidValue(ScriptCode value) const;

		std::string getValueIdentifier(ScriptCode value) const;
		ScriptCode getIdentifierValue(const std::string& identifier) const;

		bool operator== (const _NativeDataType& dt) const;
		bool operator!= (const _NativeDataType& dt) const;


	private:
		static std::map<std::string, _NativeDataType> _MappedTypes;
		static std::vector<_NativeDataType*> _TypesList;

		static std::map<std::string, _NativeDataType*> _MappedConstantByName;
		static std::map<ScriptCode, _NativeDataType*> _MappedConstantByValue;

		static const _NativeDataType* registerType(const std::string& name);
		static const _NativeDataType* registerType(const std::string& name, const std::vector<std::pair<std::string, ScriptCode>>& availableValues, const std::string& defaultName, ScriptCode defaultValue);

	public:
		static bool isValidType(const std::string& name);
		static const _NativeDataType* getType(const std::string& name);
		static const _NativeDataType* findTypeFromValue(ScriptCode value);
		static const _NativeDataType* findTypeFromValueName(const std::string& value);

		static const _NativeDataType* const Integer;
		static const _NativeDataType* const State;
		static const _NativeDataType* const Team;
		static const _NativeDataType* const Spell;
		static const _NativeDataType* const Follower;
		static const _NativeDataType* const Building;
	};
}

class DataType
{
private:
	const _NativeDataType* _type;

	DataType(const _NativeDataType* type);

public:
	DataType();

	const std::string& name() const;

	bool isValid() const;

	std::vector<std::string> availableValues() const;

	bool isValidIdentifier(const std::string& identifier) const;
	bool isValidValue(ScriptCode value) const;

	std::string getValueIdentifier(ScriptCode value) const;
	ScriptCode getIdentifierValue(const std::string& identifier) const;

	friend bool operator== (const DataType& dt0, const DataType& dt1);
	friend bool operator!= (const DataType& dt0, const DataType& dt1);

	friend bool operator! (const DataType& dt);
	operator bool() const;

public:
	static bool isValidType(const std::string& name);
	static DataType getType(const std::string& name);
	static DataType findTypeFromValue(ScriptCode value);
	static DataType findTypeFromValueName(const std::string& value);

	static DataType integer();
	static DataType state();
	static DataType team();
	static DataType spell();
	static DataType follower();
	static DataType building();
};
