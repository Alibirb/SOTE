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
	ProjectileStats(const ProjectileStats& other);
	~ProjectileStats(){}
	static ProjectileStats loadPrototype(std::string& prototypeName);	/// Loads prototype stats for the specified type
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
	ProjectileStats _stats;

	std::string _team;	/// Used to prevent friendly fire.

public:
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, std::string type, std::string team);
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, ProjectileStats stats, std::string team);
	virtual ~Projectile();
	void setPosition(osg::Vec3 position);
	virtual void onCollision();
	Damages getDamages();
	void setStats(ProjectileStats& stats)
	{
		this->_stats = stats;
	}

	std::string getTeam()
	{
		return _team;
	}

protected:
private:
};

void registerProjectileStats();


#endif // PROJECTILE_H
