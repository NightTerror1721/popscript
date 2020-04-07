#include "lang_elements.h"

#include <algorithm>

ElementAllocator::~ElementAllocator()
{
	auto it = _elems.begin();
	while (it != _elems.end())
	{
		auto ptr = *it;
		it = _elems.erase(it);
		delete ptr;
	}
}









LangElement::LangElement() :
	_uid{},
	_parent{ nullptr },
	_name{}
{
	static uintmax_t uid_gen = 0;
	_uid = ++uid_gen;
}
LangElement::~LangElement() {}

void LangElement::setParent(LangElement* parent) { _parent = parent; }
LangElement& LangElement::parent() { return *_parent; }

void LangElement::setName(const std::string& name) { _name = name; }

const LangElement& LangElement::getParent() const { return *_parent; }
bool LangElement::hasParent() const { return _parent; }

const std::string& LangElement::getName() const { return _name; }

bool operator== (const LangElement& left, const LangElement& right) { return left._uid == right._uid; }
bool operator!= (const LangElement& left, const LangElement& right) { return left._uid != right._uid; }









Namespace::Namespace() :
	LangElement{},
	_children{}
{}
Namespace::~Namespace() {}

size_t Namespace::getChildrenCount() const { return _children.size(); }
const LangElement& Namespace::getChild(const size_t idx) const { return *_children[idx]; }

void Namespace::addChild(LangElement& element) { _children.push_back(&element); }

LangElement& Namespace::operator[] (const size_t idx) { return *_children[idx]; }
const LangElement& Namespace::operator[] (const size_t idx) const { return *_children[idx]; }

const LangElement* Namespace::findChild(const std::string& name) const
{
	const auto& it = std::find_if(_children.begin(), _children.end(), [&name](LangElement* elem) {
		return elem->getName() == name;
	});
	return it == _children.end() ? nullptr : *it;
}
const LangElement* Namespace::findChildByCode(CodeValue code) const
{
	const auto& it = std::find_if(_children.begin(), _children.end(), [code](LangElement* elem) {
		switch (elem->type())
		{
			case Type::Object:
			case Type::Attribute:
			case Type::ReadOnlyAttribute:
			case Type::Function:
				return elem->as<Attribute>().getCode() == code;
		}
		return false;
	});
	return it == _children.end() ? nullptr : *it;
}

LangElement::Type Namespace::type() const { return Type::Namespace; }







Class::Class() :
	Namespace{}
{}
Class::~Class() {}

LangElement::Type Class::type() const { return Type::Class; }








Attribute::Attribute() :
	LangElement{},
	_code{}
{}
Attribute::~Attribute() {}

const ScriptCode& Attribute::getCode() const { return _code; }

void Attribute::setCode(const ScriptCode& code) { _code = code; }

LangElement::Type Attribute::type() const { return Type::Attribute; }

	








Object::Object() :
	Namespace{},
	Attribute{}
{}
Object::~Object() {}

const Class& Object::getClass() const { return getParent().as<Class>(); }

LangElement::Type Object::type() const { return Type::Object; };









ReadOnlyAttribute::ReadOnlyAttribute() :
	Attribute{}
{}
ReadOnlyAttribute::~ReadOnlyAttribute() {}

LangElement::Type ReadOnlyAttribute::type() const { return Type::ReadOnlyAttribute; }









Function::Parameter::Parameter(const Class* type, const std::string& name) :
	_type{ type },
	_name{ name }
{}

bool Function::Parameter::isInteger() const { return !_type; }
bool Function::Parameter::isClass() const { return _type; }

const Class& Function::Parameter::getTypeClass() const { return *_type; }

const std::string& Function::Parameter::getName() const { return _name; }

Function::Parameter Function::Parameter::integer(const std::string& name) { return { nullptr, name }; }
Function::Parameter Function::Parameter::of(const Class& clazz, const std::string& name) { return { &clazz, name }; }



Function::Function() :
	Attribute{},
	_pars{}
{}
Function::~Function() {}

size_t Function::getParameterCount() const { return _pars.size(); }

const Function::Parameter& Function::getParameter(const size_t idx) const { return _pars[idx]; }

void Function::addParameter(const Parameter& p) { _pars.push_back(p); }
void Function::addParameter(Parameter&& p) { _pars.push_back(std::move(p)); }

Function::Parameter& Function::operator[] (const size_t idx) { return _pars[idx]; }
const Function::Parameter& Function::operator[] (const size_t idx) const { return _pars[idx]; }

LangElement::Type Function::type() const { return Type::Function; }












ElementAllocator ElementManager::Elements;
std::map<std::string, const LangElement*> ElementManager::GlobalMap;
std::map<CodeValue, const Attribute*> ElementManager::CodeMap;

Namespace& ElementManager::newNamespace() { return *Elements.createElement<Namespace>(); }
Class& ElementManager::newClass() { return *Elements.createElement<Class>(); }
Object& ElementManager::newObject() { return *Elements.createElement<Object>(); }
Attribute& ElementManager::newAttribute() { return *Elements.createElement<Attribute>(); }
ReadOnlyAttribute& ElementManager::newReadOnlyAttribute() { return *Elements.createElement<ReadOnlyAttribute>(); }
Function& ElementManager::newFunction() { return *Elements.createElement<Function>(); }

void ElementManager::registerGlobal(const LangElement& elem)
{
	GlobalMap[elem.getName()] = &elem;
}
void ElementManager::registerCode(const Attribute& elem)
{
	CodeMap[elem.getCode()] = &elem;
}

void ElementManager::assignTo(const Namespace& parent, LangElement& elem)
{
	const_cast<Namespace&>(parent).addChild(elem);
	elem.setParent(const_cast<Namespace*>(&parent));
}

void ElementManager::assignTo(const Class& parent, Object& elem)
{
	assignTo(reinterpret_cast<const Namespace&>(parent), reinterpret_cast<LangElement&>(elem));
}
void ElementManager::assignTo(const Class& parent, Attribute& elem)
{
	assignTo(reinterpret_cast<const Namespace&>(parent), reinterpret_cast<LangElement&>(elem));
}
void ElementManager::assignTo(const Class& parent, ReadOnlyAttribute& elem)
{
	assignTo(reinterpret_cast<const Namespace&>(parent), reinterpret_cast<LangElement&>(elem));
}

void ElementManager::assignTo(const Object& parent, Attribute& elem)
{
	assignTo(reinterpret_cast<const Namespace&>(parent), reinterpret_cast<LangElement&>(elem));
}
void ElementManager::assignTo(const Object& parent, ReadOnlyAttribute& elem)
{
	assignTo(reinterpret_cast<const Namespace&>(parent), reinterpret_cast<LangElement&>(elem));
}

const Namespace& ElementManager::makeNamespace(const std::string& name)
{
	Namespace& n = newNamespace();
	n.setName(name);
	registerGlobal(n);
	return n;
}

const Class& ElementManager::makeClass(const std::string& name)
{
	Class& c = newClass();
	c.setName(name);
	registerGlobal(c);
	return c;
}

const Object& ElementManager::makeObject(const std::string& name, const Class& clazz, const ScriptCode& code)
{
	Object& o = newObject();
	o.setName(name);
	o.setCode(code);
	assignTo(clazz, o);
	registerCode(o);
	return o;
}

const Attribute& ElementManager::makeAttribute(const std::string& name, const Namespace& parent, const ScriptCode& code)
{
	Attribute& a = newAttribute();
	a.setName(name);
	a.setCode(code);
	assignTo(parent, a);
	registerCode(a);
	return a;
}

const ReadOnlyAttribute& ElementManager::makeReadOnlyAttribute(const std::string& name, const Namespace& parent, const ScriptCode& code)
{
	ReadOnlyAttribute& a = newReadOnlyAttribute();
	a.setName(name);
	a.setCode(code);
	assignTo(parent, a);
	registerCode(a);
	return a;
}

const Function& ElementManager::makeFunction(const std::string name, const std::vector<Function::Parameter>& pars)
{
	Function& f = newFunction();
	f.setName(name);
	for (const auto& par : pars)
		f.addParameter(par);
	registerCode(f);
	return f;
}

const LangElement* ElementManager::findGlobalElement(const std::string& name)
{
	const auto& it = GlobalMap.find(name);
	return it == GlobalMap.end() ? nullptr : it->second;
}

const LangElement* ElementManager::findCodeElement(CodeValue code)
{
	const auto& it = CodeMap.find(code);
	return it == CodeMap.end() ? nullptr : it->second;
}








namespace elements
{
	const LangElement* findGlobal(const std::string& name) { return ElementManager::findGlobalElement(name); }
	const LangElement* findByCode(CodeValue code) { return ElementManager::findCodeElement(code); }

	const LangElement* findChild(const LangElement& parent, const std::string& name)
	{
		using Type = LangElement::Type;
		switch (parent.type())
		{
			case Type::Namespace:
			case Type::Class:
			case Type::Object:
				return parent.as<Namespace>().findChild(name);
		}
		return nullptr;
	}
	const LangElement* findChildByCode(const LangElement& parent, CodeValue code)
	{
		using Type = LangElement::Type;
		switch (parent.type())
		{
		case Type::Namespace:
		case Type::Class:
		case Type::Object:
			return parent.as<Namespace>().findChildByCode(code);
		}
		return nullptr;
	}
}

namespace elements::namespaces
{

}

namespace elements::classes
{
	const Class& State = ElementManager::makeClass("State");
	const Class& Team = ElementManager::makeClass("Team");
	const Class& Spell = ElementManager::makeClass("Spell");
	const Class& Follower = ElementManager::makeClass("Follower");
	const Class& Building = ElementManager::makeClass("Building");
}

namespace elements::objects
{
	const Object& On = ElementManager::makeObject("on", elements::classes::State, ScriptCode::token(InstructionToken::On));
	const Object& Off = ElementManager::makeObject("off", elements::classes::State, ScriptCode::token(InstructionToken::Off));

	const Object& Blue = ElementManager::makeObject("Blue", elements::classes::Team, ScriptCode::token(CommandValueToken::Blue));
	const Object& Red = ElementManager::makeObject("Red", elements::classes::Team, ScriptCode::token(CommandValueToken::Red));
	const Object& Yellow = ElementManager::makeObject("Yellow", elements::classes::Team, ScriptCode::token(CommandValueToken::Yellow));
	const Object& Green = ElementManager::makeObject("Green", elements::classes::Team, ScriptCode::token(CommandValueToken::Green));

	const Object& Blast = ElementManager::makeObject("Blast", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Blast));
	const Object& Lightning = ElementManager::makeObject("Lightning", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::LightningBolt));
	const Object& Swarm = ElementManager::makeObject("Swarm", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::InsectPlague));
	const Object& Invisibility = ElementManager::makeObject("Invisibility", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Invisibility));
	const Object& Hypnotism = ElementManager::makeObject("Hypnotism", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Hypnotism));
	const Object& Firestorm = ElementManager::makeObject("Firestorm", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Firestorm));
	const Object& GhostArmy = ElementManager::makeObject("GhostArmy", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::GhostArmy));
	const Object& Erosion = ElementManager::makeObject("Erosion", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Erosion));
	const Object& Swamp = ElementManager::makeObject("Swamp", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Swamp));
	const Object& LandBridge = ElementManager::makeObject("LandBridge", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::LandBridge));
	const Object& AngelOfDead = ElementManager::makeObject("AngelOfDead", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::AngelOfDead));
	const Object& Earthquake = ElementManager::makeObject("Earthquake", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Earthquake));
	const Object& Flatten = ElementManager::makeObject("Flatten", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Flatten));
	const Object& Volcano = ElementManager::makeObject("Volcano", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Volcano));
	const Object& Armageddon = ElementManager::makeObject("Armageddon", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::WrathOfGod));
	const Object& Shield = ElementManager::makeObject("Shield", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Shield));
	const Object& Convert = ElementManager::makeObject("Convert", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Convert));
	const Object& Teleport = ElementManager::makeObject("Teleport", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Teleport));
	const Object& Bloodlust = ElementManager::makeObject("Bloodlust", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::Bloodlust));
	const Object& UndefinedSpell = ElementManager::makeObject("UndefinedSpell", elements::classes::Spell, ScriptCode::internal(ReadOnlyInternal::NoSpecificSpell));

	const Object& Brave = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::Brave));
	const Object& Warrior = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::Warrior));
	const Object& Preacher = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::Religious));
	const Object& Spy = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::Spy));
	const Object& Firewarrior = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::Firewarrior));
	const Object& Shaman = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::Shaman));
	const Object& UndefinedFollower = ElementManager::makeObject("Brave", elements::classes::Follower, ScriptCode::internal(ReadOnlyInternal::NoSpecificPerson));

	const Object& SmallHut = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::SmallHut));
	const Object& MediumHut = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::MediumHut));
	const Object& LargeHut = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::LargeHut));
	const Object& DrumTower = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::DrumTower));
	const Object& Temple = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::Temple));
	const Object& SpyTrain = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::SpyTrain));
	const Object& WarriorTrain = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::WarriorTrain));
	const Object& FirewarriorTrain = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::FirewarriorTrain));
	const Object& BoatHut = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::BoatHut));
	const Object& AirshipHut = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::AirshipHut));
	const Object& UndefinedBuilding = ElementManager::makeObject("SmallHut", elements::classes::Building, ScriptCode::internal(ReadOnlyInternal::NoSpecificBuilding));
}

namespace elements::attributes
{

}

namespace elements::readonly_attributes
{

}

namespace elements::functions
{

}
