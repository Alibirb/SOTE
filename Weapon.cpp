
#include "Weapon.h"
#include "Projectile.h"

#include "AngelScriptEngine.h"
#include "OwnerUpdateCallback.h"
#include "GameObjectData.h"

#include "tinyxml/tinyxml2.h"



#define WEAPON_SCRIPT_LOCATION "media/Weapons/"


WeaponStats::WeaponStats(std::string imageFilename, std::string projectileType, ProjectileStats& projectileStats, float coolDownTime)
{
	this->imageFilename = imageFilename;
	this->projectileType = projectileType;
	this->projectileStats = projectileStats;
	this->coolDownTime = coolDownTime;
}

WeaponStats::WeaponStats()
{
}

WeaponStats WeaponStats::loadPrototype(std::string prototypeName)
{
	registerWeaponStats();
	getScriptEngine()->runFile(WEAPON_SCRIPT_LOCATION + prototypeName + ".as", "WeaponStats loadStats()");
	WeaponStats stats = *(new WeaponStats(*((WeaponStats*) getScriptEngine()->getReturnObject())));
	return stats;
	//return *((WeaponStats*) getScriptEngine()->getReturnObject());
}

Weapon::Weapon(WeaponStats stats)
{
	_objectType = "Weapon";
	setStats(stats);

	loadModel(_stats.imageFilename);

	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(.75,0,0));
	_transformNode->addChild(projectileStartingTransform);
	_ready = true;
}

Weapon::Weapon(std::string type) : Weapon(WeaponStats::loadPrototype(type))
{
}
Weapon::Weapon(XMLElement* xmlElement)
{
	_objectType = "Weapon";

	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(.75,0,0));
	_transformNode->addChild(projectileStartingTransform);

	load(xmlElement);

	_ready = true;
}
Weapon::Weapon(GameObjectData* dataObj)
{
	_objectType = "Weapon";

	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(.75,0,0));
	_transformNode->addChild(projectileStartingTransform);

	load(dataObj);

	_ready = true;
}

void Weapon::load(XMLElement* xmlElement)
{
	if(xmlElement->Attribute("source"))		/// Load from external source first, then apply changes.
		load(xmlElement->Attribute("source"));

	if(xmlElement->Attribute("coolDownTime"))
		xmlElement->QueryFloatAttribute("coolDownTime", &_stats.coolDownTime);


	XMLElement* currentElement = xmlElement->FirstChildElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			loadModel(currentElement->Attribute("source"));
		else if(elementType == "projectile")
		{
			setProjectileStats(*(new ProjectileStats(currentElement)));
		}

	}
}
void Weapon::load(std::string xmlFilename)
{
	FILE *file = fopen(xmlFilename.c_str(), "rb");
	if(!file)
	{
		xmlFilename = WEAPON_SCRIPT_LOCATION + xmlFilename;
		file = fopen(xmlFilename.c_str(), "rb");
		if(!file)
			logError("Failed to open file " + xmlFilename);
	}


	XMLDocument doc(xmlFilename.c_str());
	if (doc.LoadFile(file)  != tinyxml2::XML_NO_ERROR)
	{
		logError("Failed to load file " + xmlFilename);
		logError(doc.GetErrorStr1());
	}


	XMLHandle docHandle(&doc);
	XMLElement* rootElement = docHandle.FirstChildElement().ToElement();

	load(rootElement);
}

Weapon::~Weapon()
{
}

bool Weapon::isReady()
{
	return _ready;
}

void Weapon::fire()
{
	if(!isReady())
		return;
	double angle;
	osg::Vec3 axis;
	_transformNode->getAttitude().getRotate(angle, axis);
	new Projectile(getWorldCoordinates(projectileStartingTransform)->getTrans(), osg::Vec3(cos(angle), sin(angle), 0), _stats.projectileStats, this->_team);
	if(_stats.coolDownTime != 0.0)
	{
		_ready = false;
		_coolDownTimeRemaining = _stats.coolDownTime;
	}
}

void Weapon::onUpdate(float deltaTime)
{
	if(_coolDownTimeRemaining > 0.0)
	{
		_coolDownTimeRemaining -= deltaTime;
		if(_coolDownTimeRemaining <= 0.0)
		{
			_ready = true;
			_coolDownTimeRemaining = 0.0;
		}
	}
}


void Weapon::setRotation(double angle)
{
	osg::Quat rotation;

	rotation.makeRotate(angle, osg::Vec3(0,0,1));

	_transformNode->setAttitude(rotation);
}

void Weapon::aimAt(osg::Vec3 target)
{
	osg::Vec3 position = this->getWorldPosition();
	osg::Vec3 diff = target - position;
	osg::Quat rotation;

	float angle = atan(diff.y()/diff.x());
	if (diff.x() < 0)
		angle += pi;
	if (angle < 0)
		angle += 2*pi;

	rotation.makeRotate(angle, osg::Vec3(0,0,1));

	_transformNode->setAttitude(rotation);
}

GameObjectData* Weapon::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);
	saveWeaponData(dataObj);

	return dataObj;
}
void Weapon::saveWeaponData(GameObjectData* dataObj)
{
	dataObj->addData("coolDownTime", _stats.coolDownTime);

	{
		GameObjectData* projectileData = new GameObjectData("Projectile");
		projectileData->addData("geometry", _stats.projectileStats.imageFilename);
		for(auto damage : _stats.projectileStats.damages)
		{
			GameObjectData* damageData = new GameObjectData("damage");
			damageData->addData("type", damage.type);
			damageData->addData("amount", damage.amount);
			projectileData->addChild(damageData);
		}
		dataObj->addChild(projectileData);
	}

}

void Weapon::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadWeaponData(dataObj);
}
void Weapon::loadWeaponData(GameObjectData* dataObj)
{
	/*dataObj->addData("coolDownTime", _stats.coolDownTime);

	{
		GameObjectData* projectileData = new GameObjectData("projectile");
		projectileData->addData("geometry", _stats.projectileStats.imageFilename);
		for(auto damage : _stats.projectileStats.damages)
		{
			GameObjectData* damageData = new GameObjectData("damage");
			damageData->addData("type", damage.type);
			damageData->addData("amount", damage.amount);
			projectileData->addChild(damageData);
		}
		dataObj->addChild(projectileData);
	}*/

	_stats.coolDownTime = dataObj->getFloat("coolDownTime");
	_stats.imageFilename = dataObj->getString("geometry");
	for(auto child : dataObj->getChildren())
	{
		if(child->getType() == "Projectile")
		{
			ProjectileStats projectileStats;
			projectileStats.imageFilename = child->getString("geometry");
			for(auto child : dataObj->getChildren())
			{
				if(child->getType() == "damage")
				{
					Damage dam;
					dam.amount = child->getFloat("amount");
					dam.type = child->getString("type");
					projectileStats.damages.push_back(dam);
				}
			}
			_stats.projectileStats = projectileStats;
		}
	}

}





namespace AngelScriptWrapperFunctions
{
	void WeaponStatsConstructor(WeaponStats *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) WeaponStats();
	}
	void WeaponStatsInitConstructor(std::string imageFilename, std::string projectileType, ProjectileStats& projectileStats, float coolDownTime, WeaponStats *self)
	{
		new(self) WeaponStats(imageFilename, projectileType, projectileStats, coolDownTime);
	}
	void WeaponStatsCopyConstructor(WeaponStats& other, WeaponStats* self)
	{
		new(self) WeaponStats(other.imageFilename, other.projectileType, other.projectileStats, other.coolDownTime);
	}
	void WeaponStatsDestructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((WeaponStats*)memory)->~WeaponStats();
	}
}

using namespace AngelScriptWrapperFunctions;


void registerWeaponStats()
{
	static bool registered = false;
	if(registered)
		return;

	registerProjectileStats();

	getScriptEngine()->registerObjectType("WeaponStats", sizeof(WeaponStats), asOBJ_VALUE | GetTypeTraits<WeaponStats>() );
	getScriptEngine()->registerConstructor("WeaponStats", "void f(const string &in, const string &in, const ProjectileStats &in, float)", asFUNCTION(WeaponStatsInitConstructor));
	getScriptEngine()->registerConstructor("WeaponStats", "void f()", asFUNCTION(WeaponStatsConstructor));
	getScriptEngine()->registerDestructor("WeaponStats", asFUNCTION(WeaponStatsDestructor));
	getScriptEngine()->registerConstructor("WeaponStats", "void f(const WeaponStats &in)", asFUNCTION(WeaponStatsCopyConstructor));
	getScriptEngine()->registerObjectProperty("WeaponStats", "string imageFilename", asOFFSET(WeaponStats, imageFilename));
	getScriptEngine()->registerObjectProperty("WeaponStats", "string projectileType", asOFFSET(WeaponStats, projectileType));
	getScriptEngine()->registerObjectProperty("WeaponStats", "float coolDownTime", asOFFSET(WeaponStats, coolDownTime));
	getScriptEngine()->registerObjectProperty("WeaponStats", "ProjectileStats projectileStats", asOFFSET(WeaponStats, projectileStats));

	getScriptEngine()->registerObjectMethod("WeaponStats", "void setProjectileStats(ProjectileStats &in)", asMETHOD(WeaponStats, setProjectileStats), asCALL_THISCALL);

	getScriptEngine()->registerFunction("WeaponStats loadWeaponPrototype(const string &in)", asFUNCTION(WeaponStats::loadPrototype));

	registered = true;
}
