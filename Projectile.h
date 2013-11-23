#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Item.h"
#include "Box2D/Box2D.h"



#define DEFAULT_PROJECTILE_IMAGE "test.png"

class Projectile : public Item
{
protected:
	osg::Vec3 position;
	osg::Vec3 heading;
	b2Body *physicsBody;
public:
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading);
	virtual ~Projectile();
	void setPosition(osg::Vec3 position);
	void advance();
protected:
private:
};

/**
 * Callback for the projectile, used to make it move
 */
class ProjectileNodeCallback : public osg::NodeCallback
{
public:
	Projectile *projectile;

	ProjectileNodeCallback(Projectile *projectile) {
		this->projectile = projectile;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};


#endif // PROJECTILE_H
