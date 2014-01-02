
#include "Weapon.h"
#include "Projectile.h"

#include "AngelScriptEngine.h"


WeaponStats::WeaponStats(std::string imageFilename, std::string projectileType, float coolDownTime)
{
	this->imageFilename = imageFilename;
	this->projectileType = projectileType;
	this->coolDownTime = coolDownTime;
}

WeaponStats::WeaponStats()
{
}


Weapon::Weapon(std::string type)
{
	loadStats("media/Weapons/" + type + ".as");

	sprite = new Sprite(_stats.imageFilename);
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(sprite);
	sprite->setUpdateCallback(new ItemUpdateNodeCallback(this));

	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(.75,0,0));
	transformNode->addChild(projectileStartingTransform);
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
	Vec3 axis;
	transformNode->getAttitude().getRotate(angle, axis);
	new Projectile(getWorldCoordinates(projectileStartingTransform)->getTrans(), Vec3(cos(angle), sin(angle), 0));
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

	transformNode->setAttitude(rotation);
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

	transformNode->setAttitude(rotation);
}

void Weapon::setPosition(osg::Vec3 position)
{
	this->transformNode->setPosition(position);
}
osg::Vec3 Weapon::getPosition()
{
	return transformNode->getPosition();
}
osg::Vec3 Weapon::getWorldPosition()
{
	return getWorldCoordinates(transformNode)->getTrans();
}


void Weapon::loadStats(std::string scriptFilename)
{
	registerWeaponStats();
	getScriptEngine()->runFile(scriptFilename, "WeaponStats loadStats()");
	setStats( *((WeaponStats*) getScriptEngine()->getReturnObject()));
}


namespace AngelScriptWrapperFunctions
{
	void WeaponStatsConstructor(WeaponStats *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) WeaponStats();
	}
	void WeaponStatsInitConstructor(std::string imageFilename, std::string projectileType, float coolDownTime, WeaponStats *self)
	{
		new(self) WeaponStats(imageFilename, projectileType, coolDownTime);
	}
	void WeaponStatsCopyConstructor(WeaponStats& other, WeaponStats* self)
	{
		new(self) WeaponStats(other.imageFilename, other.projectileType, other.coolDownTime);
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

	getScriptEngine()->registerObjectType("WeaponStats", sizeof(WeaponStats), asOBJ_VALUE | GetTypeTraits<WeaponStats>() );
	getScriptEngine()->registerConstructor("WeaponStats", "void f(const string &in, const string &in, float)", asFUNCTION(WeaponStatsInitConstructor));
	getScriptEngine()->registerConstructor("WeaponStats", "void f()", asFUNCTION(WeaponStatsConstructor));
	getScriptEngine()->registerDestructor("WeaponStats", asFUNCTION(WeaponStatsDestructor));
	getScriptEngine()->registerConstructor("WeaponStats", "void f(const WeaponStats &in)", asFUNCTION(WeaponStatsCopyConstructor));
	getScriptEngine()->registerObjectProperty("WeaponStats", "string imageFilename", asOFFSET(WeaponStats, imageFilename));
	getScriptEngine()->registerObjectProperty("WeaponStats", "string projectileType", asOFFSET(WeaponStats, projectileType));
	getScriptEngine()->registerObjectProperty("WeaponStats", "float coolDownTime", asOFFSET(WeaponStats, coolDownTime));

	registered = true;
}
