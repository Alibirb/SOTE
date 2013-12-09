#include "Weapon.h"

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
