#ifndef IMP_JSON_H
#define IMp_JSON_H

#include <Core/impBase.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>

IMPGEARS_BEGIN

typedef std::string JsonKey;

//-------------------------------------------------------
class IMP_API JsonValue
{
public:
	JsonValue(){}
	virtual ~JsonValue(){};
};

//-------------------------------------------------------
class IMP_API JsonPair
{
public:
	JsonPair() : value(NULL) {}
	virtual ~JsonPair() {if(value)delete value;}
	JsonKey key;
	JsonValue* value;
	
	bool operator==(const JsonPair& other) { return key==other.key; }
};

//-------------------------------------------------------
class IMP_API JsonString : public JsonValue
{
public:
	JsonString(std::string s) : value(s) {}
	std::string value;
};

//-------------------------------------------------------
class IMP_API JsonNumeric : public JsonValue
{
public:
	JsonNumeric(double d) : value(d) {}
	double value;
};

//-------------------------------------------------------
class IMP_API JsonBoolean : public JsonValue
{
public:
	JsonBoolean(bool b) : value(b) {}
	bool value;
};


class JsonObject;

//-------------------------------------------------------
class IMP_API JsonArray : public JsonValue
{
public:

	std::vector<JsonValue*> value;

	JsonValue* getValue(int i);
	JsonObject* getObject(int i);
	JsonArray* getArray(int i);
	JsonBoolean* getBoolean(int i);
	JsonNumeric* getNumeric(int i);
	JsonString* getString(int i);
};

//-------------------------------------------------------
class IMP_API JsonObject : public JsonValue
{
public:
	std::vector<JsonPair*> value;

	JsonValue* getValue(const JsonKey& key);

	JsonObject* getObject(const JsonKey& key);
};

std::string IMP_API removeSpace(const std::string& str);

bool IMP_API check(const std::string& str);

JsonValue IMP_API *parse(const std::string& str);

IMPGEARS_END

#endif // IMP_JSON_H
