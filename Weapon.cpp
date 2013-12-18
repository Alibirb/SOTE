


#include "Weapon.h"
#include "Projectile.h"

Weapon::Weapon() : Item(DEFAULT_WEAPON_IMAGE)
{
	projectileStartingTransform = new osg::PositionAttitudeTransform();
	projectileStartingTransform->setPosition(osg::Vec3(1.0,0,0));
	transformNode->addChild(projectileStartingTransform);
}

Weapon::~Weapon()
{
	//dtor
}


void Weapon::fire()
{
	//new Projectile(this->getWorldPosition(), target - getWorldPosition());
	//osg::Vec3 projCoordinates = getWorldCoordinates(projectileStartingTransform)->getTrans();
	new Projectile(getWorldCoordinates(projectileStartingTransform)->getTrans(), target - getWorldCoordinates(projectileStartingTransform)->getTrans());
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

	//rotation.makeRotate(position, target);

	float angle = atan(diff.y()/diff.x());
	if (diff.x() < 0)
		angle += pi;
	if (angle < 0)
		angle += 2*pi;
	//rotation.makeRotate(angle - pi/2, osg::Vec3(0,0,1));
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
	return getWorldCoordinates(transformNode)->getTrans();
}
