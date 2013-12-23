


#include "Weapon.h"
#include "Projectile.h"

Weapon::Weapon() : Item(DEFAULT_WEAPON_IMAGE)
{
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
