#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Item.h"


#include "Damage.h"

#define DEFAULT_PROJECTILE_IMAGE "circle.png"



/// Class to hold stats for a Projectile. Used by a Weapon to ease creation of a Projectile when needed.
class ProjectileStats
{
public:
	Damages damages;
	std::string imageFilename;

	ProjectileStats();
	ProjectileStats(XMLElement* xmlElement);
	ProjectileStats(Damages damages, std::string imageFilename);
	ProjectileStats(const ProjectileStats& other);
	~ProjectileStats(){}

	void load(XMLElement* xmlElement);
	void load(std::string xmlFilename);

	static ProjectileStats loadPrototype(std::string& prototypeName);	/// Loads prototype stats for the specified type

};


class Projectile : public Item
{
protected:
	osg::Vec3 position;
	osg::Vec3 heading;
	osg::Vec3 _velocity;
	ProjectileStats _stats;

	std::string _team;	/// Used to prevent friendly fire.

public:
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, std::string type, std::string team);
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, ProjectileStats stats, std::string team);
	virtual ~Projectile();
	Damages getDamages();
	void setStats(ProjectileStats& stats);
	std::string getTeam();

	void onUpdate(float deltaTime);
	virtual void onCollision(GameObject* other);

};


void registerProjectileStats();


#endif // PROJECTILE_H
