
#include "Weapon.h"
#include "Projectile.h"

#include "AngelScriptEngine.h"
#include "OwnerUpdateCallback.h"
#include "GameObjectData.h"


#define WEAPON_SCRIPT_LOCATION "media/Weapons/"



Weapon::Weapon(GameObjectData* dataObj)
{
	_objectType = "Weapon";

	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(.75,0,0));
	_transformNode->addChild(projectileStartingTransform);

	load(dataObj);

	_ready = true;
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
	new Projectile(getWorldCoordinates(projectileStartingTransform)->getTrans(), osg::Vec3(cos(angle), sin(angle), 0), _projectileData, this->_team);
	if(_coolDownTime != 0.0)
	{
		_ready = false;
		_coolDownTimeRemaining = _coolDownTime;
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

void Weapon::setTeam(std::string team)
{
	this->_team = team;
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
	dataObj->addData("coolDownTime", _coolDownTime);

	if(_projectileData)
	{
		/*GameObjectData* projectileData = new GameObjectData("Projectile");
		projectileData->addData("geometry", _projectileStats.imageFilename);
		for(auto damage : _projectileStats.damages)
		{
			GameObjectData* damageData = new GameObjectData("damage");
			damageData->addData("type", damage.type);
			damageData->addData("amount", damage.amount);
			projectileData->addChild(damageData);
		}
		dataObj->addData("projectile", projectileData);*/
		dataObj->addData("projectile", new GameObjectData(*_projectileData));
	}

}

void Weapon::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadWeaponData(dataObj);
}
void Weapon::loadWeaponData(GameObjectData* dataObj)
{
	_coolDownTime = dataObj->getFloat("coolDownTime");
	if(dataObj->getObject("projectile"))
	{
		/*
		ProjectileStats projectileStats;
		projectileStats.imageFilename = dataObj->getObject("projectile")->getString("geometry");
		for(auto child : dataObj->getObject("projectile")->getChildren())
		{
			if(child->getType() == "damage")
			{
				Damage dam;
				dam.amount = child->getFloat("amount");
				dam.type = child->getString("type");
				projectileStats.damages.push_back(dam);
			}
		}
		_projectileStats = projectileStats;*/
		_projectileData = new GameObjectData(*dataObj->getObject("projectile"));
	}

}




/*
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
*/
