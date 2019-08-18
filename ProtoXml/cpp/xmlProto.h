
#ifndef __xmlProto__
#define __xmlProto__

#include <tinyxml2/tinyxml2.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/text_format.h>
#include "loadConf.pb.h"
#include <map>
#include <vector>
#include <string>

using namespace ::google::protobuf;

class xmlProto
{
public:
	xmlProto();
	~xmlProto();

	void loadXml();
	void printProtoXml();
protected:
	bool loadXml(const std::string &fileName, Message *xmlMsg);
	bool loadXml(const tinyxml2::XMLElement *element, Message *xmlMsg);
	bool parseXml(const tinyxml2::XMLElement *element, const FieldDescriptor* fileDes, const Reflection* rel, Message *xmlMsg);
	bool parseRepeatedXml(const tinyxml2::XMLElement *element, const tinyxml2::XMLAttribute *attriNode, const FieldDescriptor* field, const Reflection* rel, Message *xmlMsg);
	void parseRepeatedString(const FieldDescriptor* field, const Reflection* rel, Message *xmlMsg, const std::string &content);

	std::string getFileData(const string &fileName);
	void splitStr(const std::string &content, std::vector<std::string> &strVec);
private:
	std::map<int, Message *> m_cacheMap;
};

#endif