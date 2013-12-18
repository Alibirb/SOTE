#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Item.h"
#include "Box2D/Box2D.h"



#define DEFAULT_PROJECTILE_IMAGE "circle.png"


class Projectile : public Item
{
protected:
	osg::Vec3 position;
	osg::Vec3 heading;
	b2Body *physicsBody;
	osg::Vec3 box2DToOsgAdjustment;	//adjustment between the visual and physical components
public:
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading);
	virtual ~Projectile();
	void setPosition(osg::Vec3 position);
	virtual void onCollision();
protected:
private:
};



#endif // PROJECTILE_H
