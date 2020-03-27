#include "types.h"

#include <algorithm>

_NativeDataType::_NativeDataType(const uint8_t id, const std::string& name) :
	_id{ id },
	_name{ name },
	_integerType{ true },
	_avByName{},
	_avByValue{},
	_defvalue{ 0 }
{}

_NativeDataType::_NativeDataType(
	const uint8_t id,
	const std::string& name,
	const std::vector<std::pair<std::string, ScriptCode>>& availableValues,
	const std::string& defaultName,
	ScriptCode defaultValue) :
	_id{ id },
	_name{ name },
	_integerType{ false },
	_avByName{},
	_avByValue{},
	_defvalue{ defaultValue }
{
	for (const auto& aval : availableValues)
	{
		if (!aval.first.empty())
			_avByName[aval.first] = aval.second;
		_avByValue[aval.second] = defaultName;
	}
}

const std::string& _NativeDataType::name() const { return _name; }

std::vector<std::string> _NativeDataType::availableValues() const
{
	std::vector<std::string> vec{ _avByName.size() };
	for (const auto& v : _avByName)
		vec.push_back(v.first);
	return std::move(vec);
}

bool _NativeDataType::isValidIdentifier(const std::string& identifier) const
{
	return _avByName.find(identifier) != _avByName.end();
}

bool _NativeDataType::isValidValue(ScriptCode value) const
{
	return _avByValue.find(value) != _avByValue.end();
}

std::string _NativeDataType::getValueIdentifier(ScriptCode value) const
{
	const auto& it = _avByValue.find(value);
	return it != _avByValue.end() ? it->second : "";
}

ScriptCode _NativeDataType::getIdentifierValue(const std::string& identifier) const
{
	const auto& it = _avByName.find(identifier);
	return it != _avByName.end() ? it->second : 0;
}

bool _NativeDataType::operator== (const _NativeDataType& dt) const { return _id == dt._id; }
bool _NativeDataType::operator!= (const _NativeDataType& dt) const { return _id != dt._id; }



std::map<std::string, _NativeDataType> _NativeDataType::_MappedTypes{};
std::vector<_NativeDataType*> _NativeDataType::_TypesList{};

std::map<std::string, _NativeDataType*> _NativeDataType::_MappedConstantByName{};
std::map<ScriptCode, _NativeDataType*> _NativeDataType::_MappedConstantByValue{};

const _NativeDataType* _NativeDataType::registerType(const std::string& name)
{
	if (_MappedTypes.find(name) != _MappedTypes.end())
		return nullptr;

	const uint8_t id = static_cast<uint8_t>(_MappedTypes.size());
	auto result = _MappedTypes.emplace(std::pair<std::string, _NativeDataType>{ name, { id, name } });
	if (result.second)
	{
		_NativeDataType* type = &result.first->second;
		_TypesList.push_back(type);

		for (auto& c : type->_avByName)
		{
			if (_MappedConstantByValue.find(c.second) == _MappedConstantByValue.end())
			{
				_MappedConstantByName[c.first] = type;
				_MappedConstantByValue[c.second] = type;
			}
		}

		return type;
	}
	else return nullptr;
}
const _NativeDataType* _NativeDataType::registerType(const std::string& name, const std::vector<std::pair<std::string, ScriptCode>>& availableValues, const std::string& defaultName, ScriptCode defaultValue)
{
	if (_MappedTypes.find(name) != _MappedTypes.end())
		return nullptr;

	const uint8_t id = static_cast<uint8_t>(_MappedTypes.size());
	auto result = _MappedTypes.emplace(std::pair<std::string, _NativeDataType>{ name, { id, name, availableValues, defaultName, defaultValue } });
	if (result.second)
	{
		_TypesList.push_back(&result.first->second);
		return &result.first->second;
	}
	else return nullptr;
}

bool _NativeDataType::isValidType(const std::string& name) { return _MappedTypes.find(name) != _MappedTypes.end(); }
const _NativeDataType* _NativeDataType::getType(const std::string& name)
{
	return &_MappedTypes.find(name)->second;
}
const _NativeDataType* _NativeDataType::findTypeFromValue(ScriptCode value)
{
	const auto& it = _MappedConstantByValue.find(value);
	return it == _MappedConstantByValue.end() ? nullptr : it->second;
}
const _NativeDataType* _NativeDataType::findTypeFromValueName(const std::string& value)
{
	const auto& it = _MappedConstantByName.find(value);
	return it == _MappedConstantByName.end() ? nullptr : it->second;
}






const _NativeDataType* const _NativeDataType::Integer{ _NativeDataType::registerType("Integer") };

const _NativeDataType* const _NativeDataType::State{ _NativeDataType::registerType("State", {
	{ "on", InstructionToken::On },
	{ "off", InstructionToken::Off }
}, "off", InstructionToken::Off) };

const _NativeDataType* const _NativeDataType::Team{ _NativeDataType::registerType("Team", {
	{ "Blue", CommandValueToken::Blue },
	{ "Red", CommandValueToken::Red },
	{ "Yellow", CommandValueToken::Yellow },
	{ "Green", CommandValueToken::Green }
}, "Blue", CommandValueToken::Blue) };

const _NativeDataType* const _NativeDataType::Spell{ _NativeDataType::registerType("Spell", {
	{ "", ReadOnlyInternal::Burn },
	{ "Blast", ReadOnlyInternal::Blast },
	{ "Lightning", ReadOnlyInternal::LightningBolt },
	{ "", ReadOnlyInternal::Whirlwind },
	{ "Swarm", ReadOnlyInternal::InsectPlague },
	{ "Invisibility", ReadOnlyInternal::Invisibility },
	{ "Hypnotism", ReadOnlyInternal::Hypnotism },
	{ "Firestorm", ReadOnlyInternal::Firestorm },
	{ "GhostArmy", ReadOnlyInternal::GhostArmy },
	{ "Erosion", ReadOnlyInternal::Erosion },
	{ "Swamp", ReadOnlyInternal::Swamp },
	{ "LandBridge", ReadOnlyInternal::LandBridge },
	{ "AngelOfDead", ReadOnlyInternal::AngelOfDead },
	{ "Earthquake", ReadOnlyInternal::Earthquake },
	{ "Flatten", ReadOnlyInternal::Flatten },
	{ "Volcano", ReadOnlyInternal::Volcano },
	{ "Armageddon", ReadOnlyInternal::WrathOfGod },
	{ "Shield", ReadOnlyInternal::Shield },
	{ "Convert", ReadOnlyInternal::Convert },
	{ "Teleport", ReadOnlyInternal::Teleport },
	{ "Bloodlust", ReadOnlyInternal::Bloodlust },
	{ "UndefinedSpell", ReadOnlyInternal::NoSpecificSpell }
}, "Blast", ReadOnlyInternal::Blast) };

const _NativeDataType* const _NativeDataType::Follower{ _NativeDataType::registerType("Follower", {
	{ "Brave", ReadOnlyInternal::Brave },
	{ "Warrior", ReadOnlyInternal::Warrior },
	{ "Religious", ReadOnlyInternal::Religious },
	{ "Spy", ReadOnlyInternal::Spy },
	{ "Firewarrior", ReadOnlyInternal::Firewarrior },
	{ "Shaman", ReadOnlyInternal::Shaman },
	{ "UndefinedFollower", ReadOnlyInternal::NoSpecificPerson }
}, "Brave", ReadOnlyInternal::Brave) };

const _NativeDataType* const _NativeDataType::Building{ _NativeDataType::registerType("Building", {
	{ "SmallHut", ReadOnlyInternal::SmallHut },
	{ "MediumHut", ReadOnlyInternal::MediumHut },
	{ "LargeHut", ReadOnlyInternal::LargeHut },
	{ "DrumTower", ReadOnlyInternal::DrumTower },
	{ "Temple", ReadOnlyInternal::Temple },
	{ "SpyTrain", ReadOnlyInternal::SpyTrain },
	{ "WarriorTrain", ReadOnlyInternal::WarriorTrain },
	{ "FirewarriorTrain", ReadOnlyInternal::FirewarriorTrain },
	{ "", ReadOnlyInternal::Reconversion },
	{ "", ReadOnlyInternal::WallPiece },
	{ "", ReadOnlyInternal::Gate },
	{ "BoatHut", ReadOnlyInternal::BoatHut },
	{ "", ReadOnlyInternal::BoatHut2 },
	{ "AirshipHut", ReadOnlyInternal::AirshipHut },
	{ "", ReadOnlyInternal::AirshipHut2 },
	{ "UndefinedBuilding", ReadOnlyInternal::NoSpecificBuilding }
}, "SmallHut", ReadOnlyInternal::SmallHut) };








DataType::DataType() :
	_type{ nullptr }
{}
DataType::DataType(const _NativeDataType* type) :
	_type{ type }
{}

const std::string& DataType::name() const { return _type->name(); }

bool DataType::isValid() const { return _type; }

std::vector<std::string> DataType::availableValues() const { return _type->availableValues(); }

bool DataType::isValidIdentifier(const std::string& identifier) const { return _type->isValidIdentifier(identifier); }
bool DataType::isValidValue(ScriptCode value) const { return _type->isValidValue(value); }

std::string DataType::getValueIdentifier(ScriptCode value) const { return _type->getValueIdentifier(value); }
ScriptCode DataType::getIdentifierValue(const std::string& identifier) const { return _type->getIdentifierValue(identifier); }

bool operator== (const DataType& dt0, const DataType& dt1) { return *dt0._type == *dt1._type; }
bool operator!= (const DataType& dt0, const DataType& dt1) { return *dt0._type != *dt1._type; }

bool operator! (const DataType& dt) { return !dt._type; }
DataType::operator bool() const { return _type; }


bool DataType::isValidType(const std::string& name) { return _NativeDataType::isValidType(name); }
DataType DataType::getType(const std::string& name) { return _NativeDataType::getType(name); }
DataType DataType::findTypeFromValue(ScriptCode value) { return _NativeDataType::findTypeFromValue(value); }
DataType DataType::findTypeFromValueName(const std::string& value) { return _NativeDataType::findTypeFromValueName(value); }

DataType DataType::integer() { return { _NativeDataType::Integer }; }
DataType DataType::state() { return { _NativeDataType::State }; }
DataType DataType::team() { return { _NativeDataType::Team }; }
DataType DataType::spell() { return { _NativeDataType::Spell }; }
DataType DataType::follower() { return { _NativeDataType::Follower }; }
DataType DataType::building() { return { _NativeDataType::Building }; }
