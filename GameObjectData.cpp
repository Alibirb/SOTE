#include "GameObjectData.h"


#include "globals.h"

#include "GameObject.h"

#include "tinyxml/tinyxml2.h"

#include "AngelScriptEngine.h"



GameObjectData::GameObjectData(std::string type)
{
	this->objectType = type;
}

GameObjectData::~GameObjectData()
{
	//dtor
}


void GameObjectData::addData(std::string name, int data){
	ints[name] = data;
}
void GameObjectData::addData(std::string name, float data){
	floats[name] = data;
}
void GameObjectData::addData(std::string name, bool data){
	bools[name] = data;
}
void GameObjectData::addData(std::string name, std::string data){
	strings[name] = data;
}
void GameObjectData::addData(std::string name, osg::Vec3 data){
	vectors[name] = data;
}
void GameObjectData::addChild(GameObject* child){
	_children.push_back(child->save());
}
void GameObjectData::addChildren(std::vector<GameObject*> children){
	for(auto child : children)
		_children.push_back(child->save());
}
void GameObjectData::addChild(GameObjectData* child){
	_children.push_back(child);
}
void GameObjectData::addScriptFunction(std::string name, asIScriptFunction* func){
	_scriptFunctions[name.c_str()] = func;
}
void GameObjectData::addScriptFunctions(std::unordered_map<std::string, asIScriptFunction*> functions){
	for(auto kv : functions)
		addScriptFunction(kv.first, kv.second);
}
void GameObjectData::addScriptFunction(std::string name, std::string code){
	_scriptFunctionSource[name.c_str()] = code;
}


XMLElement* GameObjectData::toXML(XMLDocument* doc)
{
	XMLElement* element = doc->NewElement(objectType.c_str());

	for(auto kv : ints) {
		element->SetAttribute(kv.first.c_str(), kv.second);
	}
	for(auto kv : floats) {
		element->SetAttribute(kv.first.c_str(), kv.second);
	}
	for(auto kv : bools) {
		element->SetAttribute(kv.first.c_str(), kv.second);
	}
	for(auto kv : strings) {
		element->SetAttribute(kv.first.c_str(), kv.second.c_str());
	}
	for(auto kv : vectors) {
		//logWarning("Exporting vectors through XML is not yet implemented.");
		std::ostringstream value;
		value << kv.second.x() << ", " << kv.second.y() << ", " << kv.second.z();
		element->SetAttribute(kv.first.c_str(), value.str().c_str());
	}
	for(auto kv : _scriptFunctionSource) {
		XMLElement* codeElement = doc->NewElement("function");
		codeElement->SetAttribute("name", kv.first.c_str());
		codeElement->SetText(kv.second.c_str());
		element->InsertEndChild(codeElement);
	}

	for(auto obj : _children) {
		element->InsertEndChild(obj->toXML(doc));
	}

	return element;

}
