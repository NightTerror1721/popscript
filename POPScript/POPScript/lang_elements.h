#pragma once

#include <list>
#include <map>

#include "utils.h"
#include "consts.h"

class LangElement;
class Namespace;
class Class;
class Object;
class Attribute;
class ReadOnlyAttribute;
class Function;

namespace
{
	class ElementAllocator
	{
	private:
		std::list<LangElement*> _elems;

	public:

		ElementAllocator() = default;
		~ElementAllocator();

		ElementAllocator(const ElementAllocator&) = delete;
		ElementAllocator(ElementAllocator&&) = delete;

		ElementAllocator& operator= (const ElementAllocator&) = delete;
		ElementAllocator& operator= (ElementAllocator&&) = delete;

	public:
		template<class _Ty>
		_Ty* createElement()
		{
			static_assert(std::is_base_of<LangElement, _Ty>::value);
			_Ty* const elem = new _Ty{};
			_elems.push_back(elem);
			return elem;
		}
	};



	class ElementManager;
}



class LangElement : public Conversor<LangElement>
{
public:
	enum class Type
	{
		Namespace,
		Class,
		Object,
		Attribute,
		ReadOnlyAttribute,
		Function
	};

private:
	uintmax_t _uid;
	LangElement* _parent;
	std::string _name;

public:
	LangElement();
	virtual ~LangElement();

	const LangElement& getParent() const;
	bool hasParent() const;

	const std::string& getName() const;

	virtual LangElement::Type type() const = 0;

	LangElement(const LangElement&) = delete;
	LangElement& operator= (const LangElement&) = delete;

protected:
	void setParent(LangElement* parent);
	LangElement& parent();

	void setName(const std::string& name);

public:
	inline bool isNamespace() const { return type() == Type::Namespace; }
	inline bool isClass() const { return type() == Type::Class; }
	inline bool isObject() const { return type() == Type::Object; }
	inline bool isAttribute() const { return type() == Type::Attribute; }
	inline bool isReadOnlyAttribute() const { return type() == Type::ReadOnlyAttribute; }
	inline bool isFunction() const { return type() == Type::Function; }

public:
	friend bool operator== (const LangElement& left, const LangElement& right);
	friend bool operator!= (const LangElement& left, const LangElement& right);

	friend ElementManager;
};



class Namespace : virtual public LangElement
{
private:
	std::vector<LangElement*> _children;

public:
	Namespace();
	virtual ~Namespace();

	size_t getChildrenCount() const;
	const LangElement& getChild(const size_t idx) const;

	const LangElement& operator[] (const size_t idx) const;

	const LangElement* findChild(const std::string& name) const;
	const LangElement* findChildByCode(CodeValue code) const;

	virtual LangElement::Type type() const override;

	Namespace(const Namespace&) = delete;
	Namespace& operator= (const Namespace&) = delete;

protected:
	void addChild(LangElement& element);
	LangElement& operator[] (const size_t idx);

public:
	friend ElementManager;
};



class Class : public Namespace
{
public:
	Class();
	~Class();

	LangElement::Type type() const override;

	Class(const Class&) = delete;
	Class& operator= (const Class&) = delete;

public:
	friend ElementManager;
};



class Attribute : virtual public LangElement
{
private:
	ScriptCode _code;

public:
	Attribute();
	virtual ~Attribute();

	const ScriptCode& getCode() const;

	virtual LangElement::Type type() const override;

	Attribute(const Attribute&) = delete;
	Attribute& operator= (const Attribute&) = delete;

protected:
	void setCode(const ScriptCode& code);

public:
	friend ElementManager;
};



class Object : public Namespace, public Attribute
{
public:
	Object();
	~Object();

	const Class& getClass() const;

	LangElement::Type type() const override;

	Object(const Object&) = delete;
	Object& operator= (const Object&) = delete;

public:
	friend ElementManager;
};



class ReadOnlyAttribute : public Attribute
{
public:
	ReadOnlyAttribute();
	~ReadOnlyAttribute();

	LangElement::Type type() const override;

	ReadOnlyAttribute(const ReadOnlyAttribute&) = delete;
	ReadOnlyAttribute& operator= (const ReadOnlyAttribute&) = delete;

public:
	friend ElementManager;
};



class Function : public Attribute
{
public:
	class Parameter
	{
	private:
		const Class* _type;
		std::string _name;

	public:
		Parameter() = default;
		Parameter(const Parameter&) = default;
		Parameter(Parameter&&) = default;

		Parameter& operator= (const Parameter&) = default;
		Parameter& operator= (Parameter&&) = default;

		bool isInteger() const;
		bool isClass() const;

		const Class& getTypeClass() const;

		const std::string& getName() const;

		static Parameter integer(const std::string& name);
		static Parameter of(const Class& clazz, const std::string& name);

	private:
		Parameter(const Class* type, const std::string& name);
	};

private:
	std::vector<Parameter> _pars;

public:
	Function();
	~Function();

	size_t getParameterCount() const;

	const Parameter& getParameter(const size_t idx) const;

	const Parameter& operator[] (const size_t idx) const;

	LangElement::Type type() const override;

	Function(const Function&) = delete;
	Function& operator= (const Function&) = delete;

protected:
	void addParameter(const Parameter& p);
	void addParameter(Parameter&& p);

	Parameter& operator[] (const size_t idx);

public:
	friend ElementManager;
};



namespace
{
	class ElementManager
	{
	public:
		ElementManager() = delete;
		ElementManager(const ElementManager&) = delete;
		ElementManager& operator= (const ElementManager&) = delete;

	private:
		static ElementAllocator Elements;
		static std::map<std::string, const LangElement*> GlobalMap;
		static std::map<CodeValue, const Attribute*> CodeMap;

		static Namespace& newNamespace();
		static Class& newClass();
		static Object& newObject();
		static Attribute& newAttribute();
		static ReadOnlyAttribute& newReadOnlyAttribute();
		static Function& newFunction();

		static void registerGlobal(const LangElement& elem);
		static void registerCode(const Attribute& elem);

		static void assignTo(const Namespace& parent, LangElement& elem);

		static void assignTo(const Class& parent, Object& elem);
		static void assignTo(const Class& parent, Attribute& elem);
		static void assignTo(const Class& parent, ReadOnlyAttribute& elem);

		static void assignTo(const Object& parent, Attribute& elem);
		static void assignTo(const Object& parent, ReadOnlyAttribute& elem);

	public:
		static void assignTo(const Namespace&, Namespace&) = delete;
		static void assignTo(const Namespace&, Class&) = delete;
		static void assignTo(const Namespace&, Function&) = delete;
		static void assignTo(const Class&, Namespace&) = delete;
		static void assignTo(const Class&, Class&) = delete;
		static void assignTo(const Class&, Function&) = delete;
		static void assignTo(const Object&, Namespace&) = delete;
		static void assignTo(const Object&, Class&) = delete;
		static void assignTo(const Object&, Object&) = delete;
		static void assignTo(const Object&, Function&) = delete;
		static void assignTo(const Attribute&, LangElement&) = delete;
		static void assignTo(const ReadOnlyAttribute&, LangElement&) = delete;
		static void assignTo(const Function&, LangElement&) = delete;

		static const Namespace& makeNamespace(const std::string& name);

		static const Class& makeClass(const std::string& name);

		static const Object& makeObject(const std::string& name, const Class& clazz, const ScriptCode& code);

		static const Attribute& makeAttribute(const std::string& name, const Namespace& parent, const ScriptCode& code);

		static const ReadOnlyAttribute& makeReadOnlyAttribute(const std::string& name, const Namespace& parent, const ScriptCode& code);

		static const Function& makeFunction(const std::string name, const std::vector<Function::Parameter>& pars);

		static const LangElement* findGlobalElement(const std::string& name);

		static const LangElement* findCodeElement(CodeValue code);
	};
}



namespace elements
{
	const LangElement* findGlobal(const std::string& name);
	const LangElement* findByCode(CodeValue code);

	const LangElement* findChild(const LangElement& parent, const std::string& name);
	const LangElement* findChildByCode(const LangElement& parent, CodeValue code);
}

namespace elements::namespaces
{

}

namespace elements::classes
{
	extern const Class& State;
	extern const Class& Team;
	extern const Class& Spell;
	extern const Class& Follower;
	extern const Class& Building;
}

namespace elements::objects
{
	/* State objects */
	extern const Object& On;
	extern const Object& Off;

	/* Team objects */
	extern const Object& Blue;
	extern const Object& Red;
	extern const Object& Yellow;
	extern const Object& Green;

	/* Spell objects */
	extern const Object& Blast;
	extern const Object& Lightning;
	extern const Object& Swarm;
	extern const Object& Invisibility;
	extern const Object& Hypnotism;
	extern const Object& Firestorm;
	extern const Object& GhostArmy;
	extern const Object& Erosion;
	extern const Object& Swamp;
	extern const Object& LandBridge;
	extern const Object& AngelOfDead;
	extern const Object& Earthquake;
	extern const Object& Flatten;
	extern const Object& Volcano;
	extern const Object& Armageddon;
	extern const Object& Shield;
	extern const Object& Convert;
	extern const Object& Teleport;
	extern const Object& Bloodlust;
	extern const Object& UndefinedSpell;

	/* Follower objects */
	extern const Object& Brave;
	extern const Object& Warrior;
	extern const Object& Preacher;
	extern const Object& Spy;
	extern const Object& Firewarrior;
	extern const Object& Shaman;
	extern const Object& UndefinedFollower;

	/* Building objects */
	extern const Object& SmallHut;
	extern const Object& MediumHut;
	extern const Object& LargeHut;
	extern const Object& DrumTower;
	extern const Object& Temple;
	extern const Object& SpyTrain;
	extern const Object& WarriorTrain;
	extern const Object& FirewarriorTrain;
	extern const Object& BoatHut;
	extern const Object& AirshipHut;
	extern const Object& UndefinedBuilding;
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
