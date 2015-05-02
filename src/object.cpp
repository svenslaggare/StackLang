#include "object.h"

std::string to_string(AccessModifiers accessModifier) {
	switch (accessModifier) {
		case AccessModifiers::Public:
			return "public";
		case AccessModifiers::Private:
			return "private";
	}
}

std::ostream& operator<<(std::ostream& os, const AccessModifiers& modifier) {
	os << to_string(modifier);
	return os;
}

//Field
Field::Field(std::string name, std::shared_ptr<Type> type, AccessModifiers accessModifier)
	: mName(name), mType(type), mAccessModifier(accessModifier) {

}

Field::Field()
	: mName(""), mType(nullptr), mAccessModifier(AccessModifiers::Public) {

}

std::string Field::name() const {
	return mName;
}

std::shared_ptr<Type> Field::type() const {
	return mType;
}

AccessModifiers Field::accessModifier() const {
	return mAccessModifier;
}

//Object
Object::Object(std::string name, std::shared_ptr<Type> type, std::unordered_map<std::string, Field> fields)
	: mName(name), mType(type), mFields(fields) {

}

Object::Object()
	: mName(""), mType(nullptr), mFields({}) {

}

std::string Object::name() const {
	return mName;
}

std::shared_ptr<Type> Object::type() const {
	return mType;
}

const std::unordered_map<std::string, Field>& Object::fields() const {
	return mFields;
}

bool Object::fieldExists(std::string name) const {
	return mFields.count(name) > 0;
}

const Field& Object::getField(std::string name) const {
	return mFields.at(name);
}