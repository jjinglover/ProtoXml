
#include "xmlProto.h"
#include <iostream>
#include <direct.h>

void xmlLoadLog(const char *arg)
{
	std::cout << arg << std::endl;
}

void xmlLoadLog(const std::string &str)
{
	xmlLoadLog(str.c_str());
}

void xmlLoadLog(const int v)
{
	std::cout << v << std::endl;
}

xmlProto::xmlProto()
{
}

xmlProto::~xmlProto()
{
}

void xmlProto::loadXml()
{
	const EnumDescriptor *enumDes = DescriptorPool::generated_pool()->FindEnumTypeByName("load.XmlInfo");
	//find load xmls setting info
	if (enumDes)
	{
		for (size_t i = 0; i < enumDes->value_count(); i++)
		{
			const EnumValueDescriptor *enumValueDes = enumDes->value(i);
			const load::LoadSet lset = enumValueDes->options().GetExtension(load::loadset);
			//get options
			if (!lset.file().empty())
			{
				const Descriptor *xmlDes = DescriptorPool::generated_pool()->FindMessageTypeByName("load." + lset.msg());
				if (xmlDes)
				{
					Message *xmlMsg = MessageFactory::generated_factory()->GetPrototype(xmlDes)->New();
					this->loadXml(lset.file(), xmlMsg);
					m_cacheMap[enumValueDes->number()] = xmlMsg;
				}
			}
		}
	}
}

void xmlProto::printProtoXml()
{
	auto it_f = m_cacheMap.find(load::Xml_school);
	if (it_f != m_cacheMap.end())
	{
		std::string str;
		//serialize message
		TextFormat::PrintToString(*it_f->second, &str);
		xmlLoadLog(str);
	}
}

bool xmlProto::loadXml(const string &fileName, Message *xmlMsg)
{
	auto data = this->getFileData(fileName);
	xmlLoadLog(data);

	tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;
	if (doc->Parse(data.c_str(), data.size()) != tinyxml2::XML_SUCCESS)
	{
		xmlLoadLog("parse " + fileName + " falied");
		return false;
	}
	const tinyxml2::XMLElement* rootEle = doc->RootElement();
	if (!rootEle){
		return true;
	}
	return loadXml(rootEle, xmlMsg);
}

bool xmlProto::loadXml(const tinyxml2::XMLElement *element, Message *xmlMsg)
{
	const Descriptor* des = xmlMsg->GetDescriptor();
	const Reflection* rel = xmlMsg->GetReflection();
	for (int i = 0; i < des->field_count(); i++)
	{
		auto filed = des->field(i);
		parseXml(element, filed, rel, xmlMsg);
	}
	return true;
}

bool xmlProto::parseXml(const tinyxml2::XMLElement *element, const FieldDescriptor* field, const Reflection* rel, Message *xmlMsg)
{
	const tinyxml2::XMLAttribute *attriNode = nullptr;
	if (field->cpp_type() != FieldDescriptor::CppType::CPPTYPE_MESSAGE){
		attriNode = element->FindAttribute(field->name().c_str());
		if (!attriNode || std::string(attriNode->Value()).empty())
		{
			return true;
		}
	}

	if (field->is_repeated()){
		// load repeated
		return parseRepeatedXml(element, attriNode, field, rel, xmlMsg);
	}

	switch (field->cpp_type())
	{
	case FieldDescriptor::CppType::CPPTYPE_INT32:
		rel->SetInt32(xmlMsg, field, attriNode->IntValue());
		break;
	case FieldDescriptor::CppType::CPPTYPE_INT64:
		rel->SetInt64(xmlMsg, field, attriNode->Int64Value());
		break;
	case FieldDescriptor::CppType::CPPTYPE_UINT32:
		rel->SetUInt32(xmlMsg, field, attriNode->IntValue());
		break;
	case FieldDescriptor::CppType::CPPTYPE_UINT64:
		rel->SetUInt64(xmlMsg, field, attriNode->Int64Value());
		break;
	case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
		rel->SetDouble(xmlMsg, field, attriNode->DoubleValue());
		break;
	case FieldDescriptor::CppType::CPPTYPE_FLOAT:
		rel->SetFloat(xmlMsg, field, attriNode->FloatValue());
		break;
	case FieldDescriptor::CppType::CPPTYPE_BOOL:
		rel->SetBool(xmlMsg, field, (bool)attriNode->IntValue());
		break;
	case FieldDescriptor::CppType::CPPTYPE_ENUM:
		//-------
		break;
	case FieldDescriptor::CppType::CPPTYPE_STRING:
		rel->SetString(xmlMsg, field, attriNode->Value());
		break;
	case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
	{
													  auto ele = element->FirstChildElement(field->name().c_str());
													  if (ele)
													  {
														  Message *newMsg = rel->MutableMessage(xmlMsg, field);
														  return loadXml(ele, newMsg);
													  }
	}
		break;
	default:
		break;
	}
	return true;
}

bool xmlProto::parseRepeatedXml(const tinyxml2::XMLElement *element, const tinyxml2::XMLAttribute *attriNode, const FieldDescriptor* field, const Reflection* rel, Message *xmlMsg)
{
	switch (field->cpp_type())
	{
	case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
	{
		auto ele = element->FirstChildElement(field->name().c_str());
		while (ele)
		{
			Message *newMsg = rel->AddMessage(xmlMsg, field);
			loadXml(ele, newMsg);
			ele = ele->NextSiblingElement();
		}							  											 
	}
		break;
	default:
		parseRepeatedString(field, rel, xmlMsg, attriNode->Value());
		break;
	}
	return true;
}

void xmlProto::parseRepeatedString(const FieldDescriptor* field, const Reflection* rel, Message *xmlMsg, const std::string &content)
{
	std::vector<std::string> strVec;
	this->splitStr(content, strVec);
	for (auto val : strVec)
	{
		switch (field->cpp_type())
		{
		case FieldDescriptor::CppType::CPPTYPE_INT32:
			rel->AddInt32(xmlMsg, field, atoi(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_INT64:
			rel->AddInt64(xmlMsg, field, atoll(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_UINT32:
			rel->AddUInt32(xmlMsg, field, atol(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_UINT64:
			rel->AddUInt64(xmlMsg, field, atoll(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
			rel->AddDouble(xmlMsg, field, atof(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_FLOAT:
			rel->AddFloat(xmlMsg, field, atof(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_BOOL:
			rel->AddBool(xmlMsg, field, (bool)atoi(val.c_str()));
			break;
		case FieldDescriptor::CppType::CPPTYPE_ENUM:
			//---
			break;
		case FieldDescriptor::CppType::CPPTYPE_STRING:
			rel->AddString(xmlMsg, field, val);
			break;
		default:
			break;
		}
	}
}

//---
std::string xmlProto::getFileData(const string &fileName)
{
	std::string data;
	char path[124] = { 0 };
	_getcwd(path, 124);
	std::string fullPath = path + string("\\") + fileName;
	auto fp = fopen(fullPath.c_str(), "r");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		if (len)
		{
			fseek(fp, 0, SEEK_SET);
			char *buff = (char *)(malloc(len + 1));
			memset(buff, '\0', len + 1);
			fread(buff, 1, len, fp);
			fclose(fp);
			data = buff;
			free(buff);
		}
	}
	return data;
}

void xmlProto::splitStr(const std::string &content, std::vector<std::string> &strVec)
{
	strVec.clear();
	char *src = (char *)malloc(content.size() + 1);
	memcpy(src, content.c_str(), content.size() + 1);
	auto cc = strtok(src, ",");
	while (cc)
	{
		strVec.push_back(cc);
		cc = strtok(nullptr, ",");
	}
	free(src);
}