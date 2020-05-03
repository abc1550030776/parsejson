#include <cstdio>
#include <string>
#include <map>
#include <string.h>
#include <sstream>
#include <vector>
#include <iostream>

const int JSON_TYPE_NUMBER = 0;
const int JSON_TYPE_STRING = 1;
const int JSON_TYPE_BOOL = 2;
const int JSON_TYPE_ARRAY = 3;
const int JSON_TYPE_OBJECT = 4;
const int JSON_TYPE_NULL = 5;

bool CheckArrayString(const std::string& str);
bool CheckObjectString(const std::string& str);

class JsonValue {
public:
	JsonValue(const std::string& str)
	{
		value = str;
	}
	virtual int type() = 0;
	virtual bool isValid() = 0;
	std::string value;
};

class Number : public JsonValue {
public:
	Number(const std::string& str): JsonValue(str)
	{}
	int type()
	{
		return JSON_TYPE_NUMBER;
	}
	bool isValid()
	{
		//�������ֵ�ʱ���Ѿ��ж����ַ����Ƿ�������
		return true;
	}
};

class JsonString : public JsonValue {
public:
	JsonString(const std::string& str): JsonValue(str)
	{}
	int type()
	{
		return JSON_TYPE_STRING;
	}
	bool isValid()
	{
		//�����ַ�����ʱ���Ѿ��ж����ַ����Ƿ����ַ���
		return true;
	}
};

class Bool : public JsonValue {
public:
	Bool(const std::string& str) : JsonValue(str)
	{
	}
	int type()
	{
		return JSON_TYPE_BOOL;
	}
	bool isValid()
	{
		//����bool��ʱ���Ѿ��ж����ַ����Ƿ���bool
		return true;
	}
};

class Array : public JsonValue {
public:
	Array(const std::string& str): JsonValue(str)
	{}
	int type()
	{
		return JSON_TYPE_ARRAY;
	}
	bool isValid()
	{
		//����ֱ�ӵ���ȫ�ֺ����ж��Ƿ��ǺϷ��������ַ���
		return CheckArrayString(value);
	}
};

class Object : public JsonValue {
public:
	Object(const std::string& str): JsonValue(str)
	{}
	int type()
	{
		return JSON_TYPE_OBJECT;
	}
	bool isValid()
	{
		//����ֱ�ӵ���ȫ�ֺ����ж��Ƿ��ǺϷ��Ķ����ַ���
		return CheckObjectString(value);
	}
};

class Null : public JsonValue {
public:
	Null(const std::string& str): JsonValue(str)
	{}
	int type()
	{
		return JSON_TYPE_NULL;
	}
	bool isValid()
	{
		//����Null��ʱ���Ѿ��ж����ַ����Ƿ���bool
		return true;
	}
};

int GetNotSpaceLength(const std::string& str, int startPos)
{
	int len = 0;
	for (; startPos + len < str.size(); ++len)
	{
		if (str[startPos + len] != ' ' && str[startPos + len] != '\t' && str[startPos + len] != '\n')
		{
			len++;
			continue;
		}
		break;
	}
	return len;
}

int GetNextNotSpacePos(const std::string& str, int startPos)
{
	int pos = startPos;
	for (; pos < str.size(); ++pos)
	{
		if (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n')
		{
			pos++;
			continue;
		}
		break;
	}
	return pos;
}

int getStringLen(const std::string& str, int startPos)
{
	bool isSpecial = false;
	for (int i = 1; i + startPos < str.size(); ++i)
	{
		if (str[startPos + i] == '\\')
		{
			if (isSpecial)
			{
				isSpecial = false;
			}
			else
			{
				isSpecial = true;
			}
			continue;
		}
		if (str[startPos + i] == '\"' && !isSpecial)
		{
			return i + 1;
		}
		isSpecial = false;
	}
	return 0;
}

bool CheckIsStringAndGetString(const std::string& str, int startPos, std::string* resultString)
{
	if (resultString == nullptr)
	{
		return false;
	}
	if (str[startPos] != '\"')
	{
		return false;
	}
	int len = getStringLen(str, startPos);
	if (len == 0)
	{
		return false;
	}
	*resultString = str.substr(startPos, len);

	return true;
}

int GetArrayLen(const std::string& str, int startPos)
{
	int leftNum = 1;
	for (int i = 1; i + startPos < str.size(); ++i)
	{
		if (str[startPos + i] == '\"')
		{
			//�ַ��������'['��']'����
			int stringLen = getStringLen(str, startPos + i);
			if (stringLen == 0)
			{
				return false;
			}
			i += stringLen;
			if (i + startPos >= str.size())
			{
				return false;
			}
		}
		if (str[startPos + i] == '[')
		{
			leftNum += 1;
		}
		if (str[startPos + i] == ']')
		{
			leftNum -= 1;
		}
		if (leftNum == 0)
		{
			return i + 1;
		}
	}
	return 0;
}

bool CheckIsArrayAndGetString(const std::string& str, int startPos, std::string* resultString)
{
	if (resultString == nullptr)
	{
		return false;
	}
	if (str[startPos] != '[')
	{
		return false;
	}
	int len = GetArrayLen(str, startPos);
	if (len == 0)
	{
		return false;
	}
	*resultString = str.substr(startPos, len);
	return true;
}

int getObjectLen(const std::string& str, int startPos)
{
	int leftNum = 1;
	for (int i = 1; i + startPos < str.size(); ++i)
	{
		if (str[startPos + i] == '\"')
		{
			//�ַ��������'{'��'}'����
			int stringLen = getStringLen(str, startPos + i);
			if (stringLen == 0)
			{
				return false;
			}
			i += stringLen;
			if (i + startPos >= str.size())
			{
				return false;
			}
		}
		if (str[startPos + i] == '{')
		{
			leftNum += 1;
		}
		if (str[startPos + i] == '}')
		{
			leftNum -= 1;
		}
		if (leftNum == 0)
		{
			return i + 1;
		}
	}
	return 0;
}

bool CheckIsObjectAndGetString(const std::string& str, int startPos, std::string* resultString)
{
	if (resultString == nullptr)
	{
		return false;
	}
	if (str[startPos] != '{')
	{
		return false;
	}
	int len = getObjectLen(str, startPos);
	if (len == 0)
	{
		return false;
	}
	*resultString = str.substr(startPos, len);
	return true;
}

bool CheckIsValueAndGetString(const std::string& str, int startPos, std::string* resultString, int& type)
{
	if (resultString == nullptr)
	{
		printf("return string is Null");
		return false;
	}

	int stringLen = 0;
	switch (str[startPos])
	{
	case '\"':
		if (!CheckIsStringAndGetString(str, startPos, resultString))
		{
			return false;
		}
		type = JSON_TYPE_STRING;
		return true;
	case 't':
	case 'f':
	{
		if (str.size() - startPos < 4)
		{
			return false;
		}
		if (!strncmp(&str[startPos], "true", 4))
		{
			*resultString = "true";
			type = JSON_TYPE_BOOL;
			return true;
		}
		if ((str.size() - startPos) >= 5 && !strncmp(&str[startPos], "false", 5))
		{
			*resultString = "false";
			type = JSON_TYPE_BOOL;
			return true;
		}
		return false;
	}
	case '[':
		if (!CheckIsArrayAndGetString(str, startPos, resultString))
		{
			return false;
		}
		type = JSON_TYPE_ARRAY;
		return true;
	case '{':
		if (!CheckIsObjectAndGetString(str, startPos, resultString))
		{
			return false;
		}
		type = JSON_TYPE_OBJECT;
		return true;
	case 'n':
	{
		if (str.size() - startPos < 4)
		{
			return false;
		}
		if (!strncmp(&str[startPos], "null", 4))
		{
			*resultString = "null";
			type = JSON_TYPE_NULL;
			return true;
		}
		return false;
	}
	default:
	{
		int len = 0;
		bool end = false;
		for (; len + startPos < str.size(); ++len)
		{
			switch (str[startPos + len])
			{
			case ' ':
			case '\n':
			case '\t':
			case ',':
			case '}':
			case ']':
				end = true;
				break;
			default:
				break;
			}
			if (end)
			{
				break;
			}
		}
		*resultString = str.substr(startPos, len);
		std::stringstream sin(*resultString);
		double d;
		char c;
		if (!(sin >> d))
			return false;
		if (sin >> c)
			return false;
		type = JSON_TYPE_NUMBER;
		return true;
	}
	}

	return true;
}

void deleteMapObject(std::map<std::string, JsonValue*>& map)
{
	for (std::map<std::string, JsonValue*>::iterator it = map.begin(); it != map.end(); ++it)
	{
		delete it->second;
	}
	map.clear();
}

bool parseObject(std::map<std::string, JsonValue*>& map, const std::string& str)
{
	int pos = 0;
	pos = GetNextNotSpacePos(str, pos);
	if (pos >= str.size())
	{
		return false;
	}
	if (str[pos] != '{')
	{
		return false;
	}
	pos = GetNextNotSpacePos(str, pos + 1);
	std::string key;
	std::string value;
	int type = JSON_TYPE_NULL;
	while (pos < str.size())
	{
		if (!CheckIsStringAndGetString(str, pos, &key))
		{
			deleteMapObject(map);
			return false;
		}
		pos += key.size();
		pos = GetNextNotSpacePos(str, pos);
		if (pos >= str.size())
		{
			deleteMapObject(map);
			return false;
		}
		if (str[pos] != ':')
		{
			deleteMapObject(map);
			return false;
		}
		pos = GetNextNotSpacePos(str, pos + 1);
		if (pos >= str.size())
		{
			deleteMapObject(map);
			return false;
		}
		if (!CheckIsValueAndGetString(str, pos, &value, type))
		{
			deleteMapObject(map);
			return false;
		}
		JsonValue* jsonValue = nullptr;
		switch (type)
		{
		case JSON_TYPE_NUMBER:
			jsonValue = new Number(value);
			break;
		case JSON_TYPE_STRING:
			jsonValue = new JsonString(value);
			break;
		case JSON_TYPE_BOOL:
			jsonValue = new Bool(value);
			break;
		case JSON_TYPE_ARRAY:
			jsonValue = new Array(value);
			break;
		case JSON_TYPE_OBJECT:
			jsonValue = new Object(value);
			break;
		case JSON_TYPE_NULL:
			jsonValue = new Null(value);
			break;
		default:
			break;
		}
		map.insert(std::make_pair(key, jsonValue));
		pos += value.size();
		pos = GetNextNotSpacePos(str, pos);
		if (pos >= str.size())
		{
			deleteMapObject(map);
			return false;
		}
		if (str[pos] != ',')
		{
			break;
		}
		pos = GetNextNotSpacePos(str, pos + 1);
	}

	if (pos >= str.size())
	{
		deleteMapObject(map);
		return false;
	}

	if (str[pos] != '}')
	{
		deleteMapObject(map);
		return false;
	}

	return true;
}

void deleteArrayObject(std::vector<JsonValue*>& vec)
{
	for (std::vector<JsonValue*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		delete *it;
	}
	vec.clear();
}

//���ַ�������������
bool parseArray(std::vector<JsonValue*>& vec, const std::string& str)
{
	int pos = 0;
	pos = GetNextNotSpacePos(str, pos);
	if (pos >= str.size())
	{
		return false;
	}
	if (str[pos] != '[')
	{
		return false;
	}
	pos = GetNextNotSpacePos(str, pos + 1);
	std::string value;
	int type = JSON_TYPE_NULL;
	while (pos < str.size())
	{
		if (!CheckIsValueAndGetString(str, pos, &value, type))
		{
			deleteArrayObject(vec);
			return false;
		}
		JsonValue* jsonValue = nullptr;
		switch (type)
		{
		case JSON_TYPE_NUMBER:
			jsonValue = new Number(value);
			break;
		case JSON_TYPE_STRING:
			jsonValue = new JsonString(value);
			break;
		case JSON_TYPE_BOOL:
			jsonValue = new Bool(value);
			break;
		case JSON_TYPE_ARRAY:
			jsonValue = new Array(value);
			break;
		case JSON_TYPE_OBJECT:
			jsonValue = new Object(value);
			break;
		case JSON_TYPE_NULL:
			jsonValue = new Null(value);
			break;
		default:
			break;
		}
		vec.push_back(jsonValue);
		pos += value.size();
		pos = GetNextNotSpacePos(str, pos);
		if (pos >= str.size())
		{
			deleteArrayObject(vec);
			return false;
		}
		if (str[pos] != ',')
		{
			break;
		}
		pos = GetNextNotSpacePos(str, pos + 1);
	}

	if (pos >= str.size())
	{
		deleteArrayObject(vec);
		return false;
	}

	if (str[pos] != ']')
	{
		deleteArrayObject(vec);
		return false;
	}
	return true;
}

//���ĳ���ַ����Ƿ��ǺϷ���Object��jsonValue
bool CheckObjectString(const std::string& str)
{
	//�Ȱ��ַ�������������map
	std::map<std::string, JsonValue*> map;
	if (!parseObject(map, str))
	{
		return false;
	}
	//�ַ����Ϸ��Ļ�map�������е�jsonValueҲӦ���ǺϷ���
	bool isValid = true;
	for (std::map<std::string, JsonValue*>::iterator it = map.begin(); it != map.end(); ++it)
	{
		if (!it->second->isValid())
		{
			isValid = false;
			break;
		}
	}
	for (std::map<std::string, JsonValue*>::iterator it = map.begin(); it != map.end(); ++it)
	{
		delete it->second;
	}
	return isValid;
}

//���ĳ���ַ����Ƿ��ǺϷ���Array��jsonValue
bool CheckArrayString(const std::string& str)
{
	//�Ȱ��ַ���������vector
	std::vector<JsonValue*> vec;
	if (!parseArray(vec, str))
	{
		return false;
	}
	//�ַ����Ϸ��Ļ�vec���������jsonValueҲӦ���ǺϷ���
	bool isValid = true;
	for (std::vector<JsonValue*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		if (!(*it)->isValid())
		{
			isValid = false;
			break;
		}
	}
	for (std::vector<JsonValue*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		delete *it;
	}
	return isValid;
}

bool ParseJson(std::map<std::string, JsonValue*>& map, const std::string& str)
{
	//�Ȱ��ַ���������map
	if (!parseObject(map, str))
	{
		return false;
	}
	//�Ϸ��Ļ��������е�ֵ���ǺϷ���
	for (std::map<std::string, JsonValue*>::iterator it = map.begin(); it != map.end(); ++it)
	{
		if (!it->second->isValid())
		{
			deleteMapObject(map);
			return false;
		}
	}
	return true;
}

int main()
{
	std::cout << "aaaaa";
	std::string str = "{\"a\":\"abc\",\"b\":{\"c\":1,\"d\":2}}";
	std::map<std::string, JsonValue*> map;
	if (ParseJson(map, str))
	{
		printf("parse sucssess");
		for (std::map<std::string, JsonValue*>::iterator it = map.begin(); it != map.end(); ++it)
		{
			printf("key %s, value %s", it->first.c_str(), it->second->value.c_str());
		}
	}
	else
	{
		printf("parse false");
	}
	deleteMapObject(map);
    return 0;
}