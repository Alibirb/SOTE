#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"

#include "Damage.h"

#define DEFAULT_PROJECTILE_IMAGE "circle.png"


/// TODO: should inherit from DangerZone, or have a DangerZone attachment
class Projectile : public GameObject
{
protected:
	//osg::Vec3 _position;
	osg::Vec3 _heading;
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
