#include "object.h"

//Field
Field::Field(std::string name, std::shared_ptr<Type> type)
	: mName(name), mType(type) {

}

Field::Field()
	: mName(""), mType(nullptr) {

}

std::string Field::name() const {
	return mName;
}

std::shared_ptr<Type> Field::type() const {
	return mType;
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