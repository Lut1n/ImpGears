#include <Descriptors/JsonReaderWriter.h>

IMPGEARS_BEGIN

JsonValue* JsonArray::getValue(int i)
{
	if(i<0 || i>=(int)value.size())
		return NULL;

	return value[i];
}

JsonObject* JsonArray::getObject(int i)
{
	return dynamic_cast<JsonObject*>(getValue(i));
}

JsonArray* JsonArray::getArray(int i)
{
	return dynamic_cast<JsonArray*>(getValue(i));
}

JsonBoolean* JsonArray::getBoolean(int i)
{
	return dynamic_cast<JsonBoolean*>(getValue(i));
}

JsonNumeric* JsonArray::getNumeric(int i)
{
	return dynamic_cast<JsonNumeric*>(getValue(i));
}

JsonString* JsonArray::getString(int i)
{
	return dynamic_cast<JsonString*>(getValue(i));
}



//-------------------------------------------------------
JsonValue* JsonObject::getValue(const JsonKey& key)
{
	for(int i=0; i<(int)value.size(); ++i)
		if(value[i]->key==key)
			return value[i]->value;

	return NULL;
}

JsonObject* JsonObject::getObject(const JsonKey& key)
{
	return dynamic_cast<JsonObject*>(getValue(key));
}

JsonArray* JsonObject::getArray(const JsonKey& key)
{
	return dynamic_cast<JsonArray*>(getValue(key));
}

JsonBoolean* JsonObject::getBoolean(const JsonKey& key)
{
	return dynamic_cast<JsonBoolean*>(getValue(key));
}

JsonNumeric* JsonObject::getNumeric(const JsonKey& key)
{
	return dynamic_cast<JsonNumeric*>(getValue(key));
}

JsonString* JsonObject::getString(const JsonKey& key)
{
	return dynamic_cast<JsonString*>(getValue(key));
}

int getEndOf(const std::string& str, int beginPos);


std::string substringPP(const std::string& str, int p1, int p2)
{
	int len = p2-p1+1;
	return str.substr(p1, len);
}

std::string substring(const std::string& str, int pos, int len)
{
	return str.substr(pos, len);
}

double asDouble(const std::string& str)
{
	std::stringstream ss(str);
	double d;
	ss >> d;
	return d;
}

bool asBoolean(const std::string& str)
{
	return str=="true" || str=="True" || str=="TRUE";
}

int first(const std::string& str, char c)
{
	for(int i=0; i<(int)str.size(); ++i)
	{
		if(str[i] == c)
			return i;
	}
	return -1;
}

int last(const std::string& str, char c)
{
	for(int i=str.size()-1; i>=0; --i)
	{
		if(str[i] == c)
			return i;
	}
	return -1;
}

int nextValue(const std::string& str)
{
	int counter = 0;
	bool inAString = false;
	for(int i=0; i<(int)str.size(); ++i)
	{
		if(str[i] == '\"')
			inAString = !inAString;
		else if(!inAString)
		{
			if(str[i] == '[' || str[i] == '{')
				counter++;
			else if(str[i] == ']' || str[i] == '}')
				counter--;
			else if(counter == 0 && str[i]==',')
				return i;
		}
	}
	return -1;
}

std::string getContained(const std::string& str, char c = '\"')
{
	// 0 1 2 3 4 5 6 7 8 9
	// " 1 2 3 4 5 6 7 8 "

	std::string result;

	int p1 = first(str, c);
	int p2 = getEndOf(str, p1); // last(str, c);

	result = substringPP(str, p1+1, p2-1);

	return result;
}

std::string removeBracket(const std::string& str)
{
	// 0 1 2 3 4 5 6 7 8 9
	// " 1 2 3 4 5 6 7 8 "

	std::string result;

	int p1 = first(str, '{');
	int p2 = getEndOf(str, p1); // last(str, '}');

	result = substring(str, p1+1, p2-p1-1);

	return result;
}

int getEndOf(const std::string& str, int beginPos)
{
	if(str[beginPos] == '[' || str[beginPos] == '{')
	{
		int pos = beginPos;
		int counter = 1;
		bool inAString = false;
		while(counter > 0)
		{
			pos++;
			if(str[pos] == '\"')
				inAString = !inAString;
			else if(!inAString)
			{
				if(str[pos] == '[' || str[pos] == '{')
					counter++;
				else if(str[pos] == ']' || str[pos] == '}')
					counter--;
			}
		}

		if( (str[beginPos]=='{' && str[pos]=='}') || (str[beginPos]=='[' && str[pos]==']') )
			return pos;
	}
	else if(str[beginPos] == '\"')
	{
		int pos = beginPos;
		pos = beginPos+1;
		while(pos < (int)str.size() && str[pos] != '\"')
			pos++;

		if(str[pos] == '\"')
			return pos;
	}
	return -1;
}

bool isNumeric(const std::string& str)
{
	return str.size()>0 && str[0] >= '0' && str[0] <= '9';
}

bool isString(const std::string& str)
{
	return str.size()>0 && str[0] == '"';
}

bool isObject(const std::string& str)
{
	return str.size()>0 && str[0] == '{';	
}

bool isArray(const std::string& str)
{
	return str.size()>0 && str[0] == '[';	
}

bool isBoolean(const std::string& str)
{
	return str.size()>0 && (str[0] == 't' || str[0] == 'T' || str[0] == 'f' || str[0] == 'F');	
}

JsonPair* parsePair(const std::string& str);
JsonObject* parseObject(const std::string& str);
JsonArray* parseArray(const std::string& str);

JsonValue* parse(const std::string& str)
{
	JsonValue* v = NULL;

	if(isBoolean(str))
		v = new JsonBoolean( asBoolean(str) );
	else if(isNumeric(str))
		v = new JsonNumeric( asDouble(str) );
	else if( isString(str) )
		v = new JsonString( getContained(str, '\"') );
	else if( isArray(str) )
		v = parseArray(str);
	else if( isObject(str) )
		v = parseObject(str);

	return v;
}

//-------------------------------------------------------
JsonPair* parsePair(const std::string& str)
{
	int p1 = first(str, ':')-1;
	int p2 = p1+2;
	std::string before = substring(str, 0, p1);
	std::string after = substring(str, p2, str.size()-p2);

	JsonPair* pair = new JsonPair();
	pair->key = getContained(before);
	pair->value = parse(after);

	return pair;
}

JsonObject* parseObject(const std::string& str)
{
	std::string contained = removeBracket(str);

	JsonObject* o = new JsonObject();

	std::string next = contained;
	while(next.size() > 0)
	{
		int pnext = nextValue(next); // first(next, ',');
		if(pnext == -1)
		{
			o->value.push_back( parsePair(substring(next, 0, next.size())) );
			next = "";
		}
		else
		{
			o->value.push_back( parsePair(substring(next, 0, pnext)) );
			next = substring(next, pnext+1, next.size()-pnext);	
		}
	}

	return o;
}

JsonArray* parseArray(const std::string& str)
{
	JsonArray* arr = new JsonArray();

	int p1 = first(str, '[');
	int p2 = getEndOf(str, p1); // last(str, ']');
	std::string contained = substringPP(str, p1+1, p2-1 );
	std::string next = contained;
	while(next.size() > 0)
	{
		int pnext = nextValue(next); // first(next, ',');
		if(pnext == -1)
		{
			arr->value.push_back( parse(substring(next, 0, next.size())) );
			next = "";
		}
		else
		{
			arr->value.push_back( parse(substring(next, 0, pnext)) );
			next = substring(next, pnext+1, next.size()-pnext);	
		}
	}
	return arr;
}

std::string removeSpace(const std::string& str)
{
	std::string result;
	bool inString = false;
	for(int i=0; i<(int)str.size(); ++i)
	{
		bool add = true;
		if(str[i] == '\"')
		{
			inString = !inString;
		}
		else if(!inString)
		{
			if(str[i] == '\n'
				|| str[i] == '\r'
				|| str[i] == ' '
				|| str[i] == '\t')
				add = false;
		}

		if(add)
			result.push_back(str[i]);
	}

	return result;
}

bool check(const std::string& string)
{
	std::string str = removeSpace(string);

	std::vector<char> symbolPile;
	bool inString = false;

	for(int i=0; i<(int)str.size(); ++i)
	{
		if(str[i] == '\"')
		{
			inString = !inString;
		}
		else if(!inString)
		{
			if(str[i] == '[')
			{
				symbolPile.push_back('[');
			}
			else if(str[i] == ']')
			{
				if(symbolPile.back() != '[')
					return false;

				symbolPile.pop_back();
			}
			else if(str[i] == '{')
			{
				symbolPile.push_back('{');
			}
			else if(str[i] == '}')
			{
				if(symbolPile.back() != '{')
					return false;

				symbolPile.pop_back();
			}
		}
	}
	return true;
}

IMPGEARS_END
