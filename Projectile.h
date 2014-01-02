#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Item.h"
#include "Box2D/Box2D.h"

#include "Damage.h"

#define DEFAULT_PROJECTILE_IMAGE "circle.png"


class ProjectileStats
{
public:
	ProjectileStats();
	ProjectileStats(Damages damages, std::string imageFilename);
	Damages damages;
	std::string imageFilename;
};


class Projectile : public Item
{
protected:
	osg::Vec3 position;
	osg::Vec3 heading;
	b2Body *physicsBody;
	osg::Vec3 box2DToOsgAdjustment;	/// adjustment between the visual and physical components
	//float _damage = 2.0;
	ProjectileStats _stats;


public:
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, std::string type="DefaultProjectile");
	virtual ~Projectile();
	void setPosition(osg::Vec3 position);
	virtual void onCollision();
	Damages getDamages();
	void loadStats(std::string scriptFilename);
	void setStats(ProjectileStats& stats)
	{
		this->_stats = stats;
	}

protected:
private:
};

void registerProjectileStats();


#endif // PROJECTILE_H
