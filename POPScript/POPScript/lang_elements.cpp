#include "lang_elements.h"

LangElement::LangElement() :
	_parent{ nullptr },
	_name{}
{}
LangElement::~LangElement() {}

void LangElement::setParent(LangElement* parent) { _parent = parent; }
LangElement& LangElement::parent() { return *_parent; }

void LangElement::setName(const std::string& name) { _name = name; }

const LangElement& LangElement::parent() const { return *_parent; }
bool LangElement::hasParent() const { return _parent; }

const std::string& LangElement::name() const { return _name; }
