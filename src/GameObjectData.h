#ifndef GAMEOBJECTDATA_H
#define GAMEOBJECTDATA_H


#include <unordered_map>
#include <vector>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>

#include <string>



namespace YAML
{
	class Node;
	class Emitter;
}

class asIScriptFunction;

class GameObjectData;

class Saveable
{
public:
	std::string _objectType;
	std::string _prototype;

public:
	std::string getType() {
		return _objectType;
	}
	std::string getPrototype() {
		return _prototype;
	}

	virtual GameObjectData* save()=0;
	virtual void load(GameObjectData* dataObj)=0;

	void saveSaveableVariables(GameObjectData* dataObj);	/// Saves the variables declared in "Saveable"
	void loadSaveableVariables(GameObjectData* dataObj);
};


/// NOTE: For example of how OSG deals with serialization, see osgDB/DotOsgWrapper.cpp (in writeObject)

/// NOTE: See the HPL1 source for example of saving/loading

/// NOTE: *sigh* I suppose I'll have to unit test this at some point.
/// TODO: use preprocessor macros to define these getter/setter methods, if possible


/// Class for a GameObject to store/retrieve its data
/// This is used to facilitate saving/loading and modification.
/// Note that it may be used to store variables from things other than GameObjects (like state machines)
class GameObjectData
{
private:
	std::string objectType;
	std::string _prototype;

	std::unordered_map<std::string, int> ints;
	std::unordered_map<std::string, double> floats;
	std::unordered_map<std::string, bool> bools;
	std::unordered_map<std::string, std::string> strings;
	std::unordered_map<std::string, osg::Vec3> _vec3s;
	std::unordered_map<std::string, osg::Vec4> _vec4s;
	std::unordered_map<std::string, asIScriptFunction*> _scriptFunctions;
	std::unordered_map<std::string, std::string> _scriptFunctionSources;

	std::unordered_map<std::string, GameObjectData*> _objects;	/// Other objects
	std::unordered_map<std::string, std::vector<GameObjectData*>> _objectLists;	/// Lists of objects. TODO: maybe it should be std::list?
	std::unordered_map<std::string, std::unordered_map<std::string, GameObjectData*>> _objectMaps;

public:
	GameObjectData();
	GameObjectData(YAML::Node node);
	virtual ~GameObjectData();

	void addData(std::string name, int data);
	void addData(std::string name, float data);
	void addData(std::string name, double data);
	void addData(std::string name, bool data);
	void addData(std::string name, std::string data);
	void addData(std::string name, osg::Vec3 data);
	void addData(std::string name, osg::Vec4 data);
	void addData(std::string name, osg::Quat data);
	void addData(std::string name, GameObjectData* data);
	void addData(std::string name, std::vector<GameObjectData*> data);
	void addData(std::string name, std::unordered_map<std::string, GameObjectData*> data);
	void addData(std::string name, Saveable* data);
	//void addData(std::string name, std::vector<Saveable*> objectList);

	template<class T>
	void addData(std::string name, std::vector<T*> objectList)
	{
		for(auto object : objectList)
			_objectLists[name].push_back(object->save());
	}

	/// Add a map of objects inheriting from Saveable
	template<class T>
	void addData(std::string name, std::unordered_map<std::string, T*> objectMap)
	{
		for(auto kv : objectMap)
		{
			_objectMaps[name][kv.first] = kv.second->save();
		}
	}

	void addScriptFunction(std::string name, asIScriptFunction* func);
	void addScriptFunctions(std::unordered_map<std::string, asIScriptFunction*> functions);

	void addScriptFunction(std::string name, std::string source);	/// saves the source code of a function.
	void addScriptFunctions(std::unordered_map<std::string, std::string> functions);



	int getInt(std::string name);
	double getFloat(std::string name);
	bool getBool(std::string name);
	std::string getString(std::string name);
	osg::Vec3 getVec3(std::string name);
	osg::Vec4 getVec4(std::string name);
	osg::Quat getQuat(std::string name);
	std::string getFunctionSource(std::string name);
	std::unordered_map<std::string, std::string> getFunctionSources();
	std::unordered_map<std::string, int> getAllInts();
	std::unordered_map<std::string, double> getAllFloats();
	std::unordered_map<std::string, bool> getAllBools();
	std::unordered_map<std::string, std::string> getAllStrings();
	std::unordered_map<std::string, osg::Vec3> getAllVec3s();
	std::unordered_map<std::string, osg::Vec4> getAllVec4s();
	GameObjectData* getObject(std::string name);
	std::vector<GameObjectData*> getObjectList(std::string name);	/// get the specifed object list variable
	std::unordered_map<std::string, GameObjectData*> getObjectMap(std::string name);

	bool hasInt(std::string name);
	bool hasFloat(std::string name);
	bool hasVec3(std::string name);
	bool hasString(std::string name);
	bool hasFunctionSource(std::string name);

	std::string getType();
	void setType(std::string type);

	std::string getPrototype();
	void setPrototype(std::string prototype);

	YAML::Emitter& toYAML(YAML::Emitter& emitter);
	void fromYAML(YAML::Node node);

	static YAML::Node loadYamlFile(std::string filenameAndNodeLocation);	/// Loads (part of) a YAML file into nodes. You can pass in a string containing the filename and location in the file, or just the filename.
	void saveAsYamlFile(std::string filename);

	static void testYamlImportExport(std::string importFilename, std::string exportFilename);	/// Helper testing function. Imports a YAML file, then exports it. Check (manually) to ensure no data was lost

	void simplify();	/// Simplifies the data object for a more condensed output by removing all attributes that show no difference from the prototype.
	void simplify(GameObjectData* prototypeData, std::string prototypeLocation);

	bool isEmpty();

};



#endif // GAMEOBJECTDATA_H
