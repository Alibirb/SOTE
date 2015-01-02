#include "GameObjectData.h"


#include "globals.h"

#include "GameObject.h"

#include "AngelScriptEngine.h"

#include "yaml-cpp/yaml.h"

#include <osgDB/FileUtils>



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
void GameObjectData::addData(std::string name, double data){
	floats[name] = data;
}
void GameObjectData::addData(std::string name, bool data){
	bools[name] = data;
}
void GameObjectData::addData(std::string name, std::string data){
	strings[name] = data;
}
void GameObjectData::addData(std::string name, osg::Vec3 data){
	_vec3s[name] = data;
}
void GameObjectData::addData(std::string name, osg::Vec4 data){
	_vec4s[name] = data;
}
void GameObjectData::addData(std::string name, osg::Quat data){
	_vec4s[name] = osg::Vec4(data.x(), data.y(), data.z(), data.w());
}
void GameObjectData::addData(std::string name, GameObjectData* data) {
	_objects[name] = data;
}
void GameObjectData::addData(std::string name, std::vector<GameObjectData*> data) {
	_objectLists[name] = data;
}
void GameObjectData::addData(std::string name, std::unordered_map<string, GameObjectData*> data) {
	_objectMaps[name] = data;
}
void GameObjectData::addData(std::string name, Saveable* data) {
	_objects[name] = data->save();
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
void GameObjectData::addScriptFunctions(std::unordered_map<std::string, std::string> functions){
	for(auto kv : functions)
		addScriptFunction(kv.first, kv.second);
}

int GameObjectData::getInt(std::string name) {
	return ints[name];
}
double GameObjectData::getFloat(std::string name) {
	if(floats.count(name.c_str()))
		return floats[name];
	else if(ints.count(name.c_str()))	// May have been stored as an int instead
		return ints[name];
}
bool GameObjectData::getBool(std::string name) {
	return bools[name];
}
std::string GameObjectData::getString(std::string name) {
	return strings[name];
}
osg::Vec3 GameObjectData::getVec3(std::string name) {
	return _vec3s[name];
}
osg::Vec4 GameObjectData::getVec4(std::string name) {
	return _vec4s[name];
}
osg::Quat GameObjectData::getQuat(std::string name) {
	if(!_vec4s.count(name.c_str()))
		return osg::Quat(0, 0, 0, 1);	/// If the Quat isn't found, return one corresponding to no rotation.
	else
		return osg::Quat(_vec4s[name]);
}
std::string GameObjectData::getFunctionSource(std::string name) {
	return _scriptFunctionSource[name];
}
std::unordered_map<std::string, std::string> GameObjectData::getFunctionSources() {
	return _scriptFunctionSource;
}
GameObjectData* GameObjectData::getObject(std::string name) {
	return _objects[name];
}
std::vector<GameObjectData*> GameObjectData::getObjectList(std::string name) {
	return _objectLists[name];
}
std::unordered_map<string, GameObjectData*> GameObjectData::getObjectMap(std::string name) {
	return _objectMaps[name];
}

std::unordered_map<std::string, int> GameObjectData::getAllInts() {
	return ints;
}
std::unordered_map<std::string, double> GameObjectData::getAllFloats() {
	return floats;
}
std::unordered_map<std::string, bool> GameObjectData::getAllBools() {
	return bools;
}
std::unordered_map<std::string, std::string> GameObjectData::getAllStrings() {
	return strings;
}
std::unordered_map<std::string, osg::Vec3> GameObjectData::getAllVec3s() {
	return _vec3s;
}
std::unordered_map<std::string, osg::Vec4> GameObjectData::getAllVec4s() {
	return _vec4s;
}

bool GameObjectData::hasInt(std::string name) {
	return ints.count(name.c_str());
}
bool GameObjectData::hasFloat(std::string name) {
	if(floats.count(name.c_str()))
		return true;
	else if(ints.count(name.c_str()))	/// May have been stored as an int instead
		return true;
	else
		return false;
}
bool GameObjectData::hasVec3(std::string name) {
	return _vec3s.count(name.c_str());
}
bool GameObjectData::hasString(std::string name) {
	return strings.count(name.c_str());
}
bool GameObjectData::hasFunctionSource(std::string name) {
	return _scriptFunctionSource.count(name.c_str());
}

std::string GameObjectData::getType() {
	return objectType;
}



YAML::Emitter& GameObjectData::toYAML(YAML::Emitter& emitter)
{
	emitter << YAML::BeginMap;

	emitter << YAML::Key << "dataType";
	emitter << YAML::Value << objectType.c_str();

	for(auto kv : ints) {
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value << kv.second;
	}
	for(auto kv : floats) {
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value << kv.second;
	}
	for(auto kv : bools) {
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value << kv.second;
	}
	for(auto kv : strings) {
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value << kv.second;
	}
	for(auto kv : _vec3s)
	{
		// TODO: should specialize YAML convert template class for Vec3
		// See http://code.google.com/p/yaml-cpp/wiki/Tutorial
		osg::Vec3 theVector = kv.second;

		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value;
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq;
		emitter << theVector.x();
		emitter << theVector.y();
		emitter << theVector.z();
		emitter << YAML::EndSeq;
		emitter << YAML::Block;
	}
	for(auto kv : _vec4s)
	{
		// TODO: should specialize YAML convert template class for Vec4
		// See http://code.google.com/p/yaml-cpp/wiki/Tutorial
		osg::Vec4 theVector = kv.second;

		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value;
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq;
		emitter << theVector.x();
		emitter << theVector.y();
		emitter << theVector.z();
		emitter << theVector.w();
		emitter << YAML::EndSeq;
		emitter << YAML::Block;
	}
	for(auto kv : _objects)
	{
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value;
		kv.second->toYAML(emitter);
	}

	if(!_scriptFunctionSource.empty())
	{
		emitter << YAML::Key << "functions";
		emitter << YAML::Value;
		emitter << YAML::BeginMap;
		for(auto kv : _scriptFunctionSource)
		{
			emitter << YAML::Key << kv.first;
			emitter << YAML::Value << YAML::Literal << kv.second;
		}
		emitter << YAML::EndMap;
	}

	for(auto kv : _objectLists)
	{
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value;
		emitter << YAML::BeginSeq;
		for(auto obj : kv.second) {
			obj->toYAML(emitter);
		}
		emitter << YAML::EndSeq;
	}

	for(auto kv : _objectMaps)
	{
		emitter << YAML::Key << kv.first.c_str();
		emitter << YAML::Value;
		emitter << YAML::BeginMap;
		for(auto kv2 : kv.second)
		{
			emitter << YAML::Key << kv2.first.c_str();
			emitter << YAML::Value;
			kv2.second->toYAML(emitter);
		}
		emitter << YAML::EndMap;
	}

	emitter << YAML::EndMap;

	return emitter;
}




GameObjectData::GameObjectData(YAML::Node node)
{
	this->fromYAML(node);
}

bool isVec3(YAML::Node node)
{
	if(node.size() != 3)
		return false;
	if(node.IsMap() && (node["x"] && node["y"] && node["z"]))
		return true;
	if(node.IsSequence())
	{
		/// Check that the first element is a float.
		if(!node[0].IsScalar())
			return false;
		float floatValue;
		if(YAML::convert<float>::decode(node[0], floatValue))
			return true;
	}
	return false;
}
osg::Vec3 toVec3(YAML::Node node)
{
	osg::Vec3 vec;
	if(node.IsMap())
		vec.set(node["x"].as<double>(), node["y"].as<double>(), node["z"].as<double>());
	else if(node.IsSequence())
		vec.set(node[0].as<double>(), node[1].as<double>(), node[2].as<double>());
	return vec;
}

bool isVec4(YAML::Node node)
{
	if(node.size() != 4)
		return false;
	if(node.IsMap())
	{
		if(node["x"] && node["y"] && node["z"] && node["w"])
			return true;
		if(node["r"] && node["g"] && node["b"] && node["a"])
			return true;
	}
	if(node.IsSequence())
	{
		/// Check that the first element is a float.
		if(!node[0].IsScalar())
			return false;
		float floatValue;
		if(YAML::convert<float>::decode(node[0], floatValue))
			return true;
	}
	return false;
}
osg::Vec4 toVec4(YAML::Node node)
{
	osg::Vec4 vec;
	if(node.IsMap())
	{
		if( node["x"] && node["y"] && node["z"] && node["w"] )
			vec.set(node["x"].as<double>(), node["y"].as<double>(), node["z"].as<double>(), node["w"].as<double>());
		else if( node["r"] && node["g"] && node["b"] && node["a"] )
			vec.set(node["r"].as<double>(), node["g"].as<double>(), node["b"].as<double>(), node["a"].as<double>());
	}
	else if(node.IsSequence())
	{
		vec.set(node[0].as<double>(), node[1].as<double>(), node[2].as<double>(), node[3].as<double>());
	}
	return vec;
}

bool isQuat(YAML::Node node)
{
	if(node.size() != 4)
		return false;
	if(!node["x"] || !node["y"] || !node["z"] || !node["w"])
		return false;
	return true;
}
osg::Quat toQuat(YAML::Node node)
{
	osg::Quat quat;
	quat.set(node["x"].as<double>(), node["y"].as<double>(), node["z"].as<double>(), node["w"].as<double>());
	return quat;
}



void GameObjectData::fromYAML(YAML::Node node)
{
	try {
		objectType = node["dataType"].as<std::string>();
	} catch (YAML::TypedBadConversion<std::string>& e) {
		std::cout << "YAML claims it couldn't read dataType." << std::endl;
	}

	/// NOTE: the node corresponding to GameObjectData should always be of Map type
	for(YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string key;
		key = it->first.as<std::string>();
		if(key == "dataType")
			continue;	// dealt with at the start. dataType is stored seperately from the other properties.


		switch(it->second.Type())	// Check what kind of node this data is
		{
		case YAML::NodeType::Sequence:
			if(isVec4(it->second))	/// Could be a Vec4
				this->addData(key, toVec4(it->second));
			else if(isVec3(it->second))	/// Could be a Vec3
				this->addData(key, toVec3(it->second));
			else
			{
				/// May contain a list of items
				for(int i = 0; i < it->second.size(); i++)
				{
					/// Iterate through all nodes in the sequence and add them
					_objectLists[key].push_back(new GameObjectData(it->second[i]));
				}
			}
			break;
		case YAML::NodeType::Map:
			// Map type
			if(isQuat(it->second))	// Could be a Quat
				this->addData(key, toQuat(it->second));
			else if(isVec3(it->second))	// Could be a Vec3
				this->addData(key, toVec3(it->second));
			else if(it->first.as<std::string>() == "functions")	// Or functions
			{
				for(YAML::const_iterator funcIt = it->second.begin(); funcIt != it->second.end(); ++funcIt)
				{
					addScriptFunction(funcIt->first.as<std::string>(), funcIt->second.as<std::string>());	/// Assuming that the function is source code, not byte code.
				}
			}
			else if(it->second["dataType"])
			{
				// Could be GameObjectData
				addData(key, new GameObjectData(it->second));
			}
			else	/// Probably a map of GameObjectData then
			{
				/// if it's not a map of GameObjectData, this will fail.
				/// Reasons a node might be incorrectly-identified:
				///		it's a new data type that's not yet recognized
				///		typo: yaml-cpp will not recognize a mapping unless there's a space after the colon.
				//addData(key, new GameObjectData(it->second));
				unordered_map<string, GameObjectData*> objMap;
				for(YAML::const_iterator objIt = it->second.begin(); objIt != it->second.end(); ++objIt)
				{
					objMap[objIt->first.as<std::string>()] = new GameObjectData(objIt->second);
				}
				addData(key, objMap);
			}
			break;
		case YAML::NodeType::Scalar:
			// Simple primitive data type

			/// Figure out what type it is.
			double floatValue;
			bool boolValue;
			if(YAML::convert<double>::decode(it->second, floatValue))
				this->addData(key, floatValue);
			else if(YAML::convert<bool>::decode(it->second, boolValue))
				this->addData(key, boolValue);
			else this->addData(key, it->second.as<std::string>());
			break;

		case YAML::NodeType::Null:
			break;
		}
	}

}


void GameObjectData::testYamlImportExport(std::string importFilename, std::string exportFilename)
{
	GameObjectData* data = new GameObjectData(YAML::LoadFile(importFilename));

	// Simple test, only ensures that GameObjectData is imported and exported consistently
	YAML::Emitter emitter;
	data->toYAML(emitter);
	std::ofstream fout(exportFilename);
	fout << emitter.c_str();


	// More advanced test: also tests whether objects are consistent in their implementations (loading the same variables they save)
	// TODO

}


