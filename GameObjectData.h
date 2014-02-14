#ifndef GAMEOBJECTDATA_H
#define GAMEOBJECTDATA_H


#include <unordered_map>
#include <vector>

#include <osg/Vec3>




namespace tinyxml2
{
	class XMLElement;
	class XMLDocument;
}
using namespace tinyxml2;

class asIScriptFunction;

class GameObject;


/// NOTE: For example of how OSG deals with serialization, see osgDB/DotOsgWrapper.cpp (in writeObject)


/// NOTE: *sigh* I suppose I'll have to unit test this at some point.


/// Class for a GameObject to store/retrieve its data
/// This is used to facilitate saving/loading and modification.
/// Note that it may be used to store variables from things other than GameObjects (like state machines)
class GameObjectData
{
private:
	std::string objectType;
	std::unordered_map<std::string, int> ints;
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, bool> bools;
	std::unordered_map<std::string, std::string> strings;
	std::unordered_map<std::string, osg::Vec3> vectors;
	//std::unordered_map<std::string, osg::PositionAttitudeTransform> transforms;	// TODO
	std::vector<GameObjectData*> _children;
	std::unordered_map<std::string, asIScriptFunction*> _scriptFunctions;
	std::unordered_map<std::string, std::string> _scriptFunctionSource;

public:
	GameObjectData(std::string type);
	virtual ~GameObjectData();

	void addData(std::string name, int data);
	void addData(std::string name, float data);
	void addData(std::string name, bool data);
	void addData(std::string name, std::string data);
	void addData(std::string name, osg::Vec3 data);
	void addChild(GameObjectData* child);
	void addChild(GameObject* child);
	void addChildren(std::vector<GameObject*> children);

	void addScriptFunction(std::string name, asIScriptFunction* func);
	void addScriptFunctions(std::unordered_map<std::string, asIScriptFunction*> functions);

	void addScriptFunction(std::string name, std::string source);	/// saves the source code of a function.

	/*TODO:
	int getInt(std::string name);
	float getFloat(std::string name);
	bool getBool(std::string name);
	std::string getString(std::string name);
	osg::Vec3 getVector(std::string name);
	std::vector<GameObject*> getChildren();
	*/

	XMLElement* toXML(XMLDocument* doc);

};

#endif // GAMEOBJECTDATA_H
