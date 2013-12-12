


#include "Weapon.h"
#include "Projectile.h"

Weapon::Weapon() : Item(DEFAULT_WEAPON_IMAGE)
{
	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(0,.5,0));
	transformNode->addChild(projectileStartingTransform);
}

Weapon::~Weapon()
{
	//dtor
}


void Weapon::fire()
{
	//new Projectile(this->getWorldPosition(), target - getWorldPosition());
	new Projectile(getWorldCoordinates(projectileStartingTransform), target - getWorldCoordinates(projectileStartingTransform));
}


void Weapon::aimAt(osg::Vec3 target)
{
	osg::Vec3 position = this->getWorldPosition();
	osg::Vec3 diff = target - position;
	osg::Quat rotation;

	//rotation.makeRotate(position, target);

	float angle = atand(diff.y()/diff.x());
	if (diff.x() < 0)
		angle = 180 - angle;
	if (angle < 0)
		angle += 360;
	rotation.makeRotate(angle, osg::Vec3(0,0,1));

	transformNode->setAttitude(rotation);

	this->target = target;
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
	//return transformNode->getPosition() + ( (osg::PositionAttitudeTransform*) (transformNode->getParent(0)))->getPosition();
	return getWorldCoordinates(transformNode);
}
