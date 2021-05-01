#ifndef IMP_JSON_H
#define IMP_JSON_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Descriptors/Export.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>

IMPGEARS_BEGIN

typedef std::string JsonKey;

//-------------------------------------------------------
class IG_DESC_API JsonValue
{
public:
	JsonValue(){}
	virtual ~JsonValue(){};
};

//-------------------------------------------------------
class IG_DESC_API JsonPair
{
public:
	JsonPair() : value(NULL) {}
	virtual ~JsonPair() {if(value)delete value;}
	JsonKey key;
	JsonValue* value;
	
	bool operator==(const JsonPair& other) { return key==other.key; }
};

//-------------------------------------------------------
class IG_DESC_API JsonString : public JsonValue
{
public:
	JsonString(std::string s) : value(s) {}
	std::string value;
};

//-------------------------------------------------------
class IG_DESC_API JsonNumeric : public JsonValue
{
public:
	JsonNumeric(double d) : value(d) {}
	double value;
};

//-------------------------------------------------------
class IG_DESC_API JsonBoolean : public JsonValue
{
public:
	JsonBoolean(bool b) : value(b) {}
	bool value;
};


class JsonObject;

//-------------------------------------------------------
class IG_DESC_API JsonArray : public JsonValue
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
class IG_DESC_API JsonObject : public JsonValue
{
public:
	std::vector<JsonPair*> value;

	JsonValue* getValue(const JsonKey& key);

	JsonObject* getObject(const JsonKey& key);
	JsonArray* getArray(const JsonKey& key);
	JsonBoolean* getBoolean(const JsonKey& key);
	JsonNumeric* getNumeric(const JsonKey& key);
	JsonString* getString(const JsonKey& key);
};

std::string IG_DESC_API removeSpace(const std::string& str);

bool IG_DESC_API check(const std::string& str);

JsonValue IG_DESC_API *parse(const std::string& str);

IMPGEARS_END

#endif // IMP_JSON_H
