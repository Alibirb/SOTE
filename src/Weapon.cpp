
#include "Weapon.h"
#include "Projectile.h"

#include "AngelScriptEngine.h"
#include "OwnerUpdateCallback.h"
#include "GameObjectData.h"


Weapon::Weapon(osg::Group* parentNode) : Attachment(parentNode)
{
	_objectType = "Weapon";

	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(.75,0,0));
	_transformNode->addChild(projectileStartingTransform);

	_ready = true;
}

Weapon::Weapon(GameObjectData* dataObj, osg::Group* parentNode) : Weapon(parentNode)
{
	load(dataObj);
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
	GameObjectData* dataObj = new GameObjectData();

	saveSaveableVariables(dataObj);
	saveGameObjectVariables(dataObj);
	saveWeaponData(dataObj);

	return dataObj;
}
void Weapon::saveWeaponData(GameObjectData* dataObj)
{
	dataObj->addData("coolDownTime", _coolDownTime);

	if(_projectileData)
		dataObj->addData("projectile", new GameObjectData(*_projectileData));

}

void Weapon::load(GameObjectData* dataObj)
{
	loadSaveableVariables(dataObj);
	loadGameObjectVariables(dataObj);
	loadWeaponData(dataObj);
}
void Weapon::loadWeaponData(GameObjectData* dataObj)
{
	_coolDownTime = dataObj->getFloat("coolDownTime");
	if(dataObj->getObject("projectile"))
		_projectileData = new GameObjectData(*dataObj->getObject("projectile"));
}

