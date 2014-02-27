#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Item.h"


#include "Damage.h"

#define DEFAULT_PROJECTILE_IMAGE "circle.png"



class Projectile : public Item
{
protected:
	osg::Vec3 position;
	osg::Vec3 heading;
	osg::Vec3 _velocity;

	std::string _team;	/// Used to prevent friendly fire.

	Damages _damages;

public:
	Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, GameObjectData* dataObj, std::string team);
	virtual ~Projectile();
	Damages getDamages();
	std::string getTeam();

	void onUpdate(float deltaTime);
	virtual void onCollision(GameObject* other);

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveProjectileData(GameObjectData* dataObj);
	void loadProjectileData(GameObjectData* dataObj);

};


//void registerProjectileStats();


#endif // PROJECTILE_H
