#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Item.h"
#ifdef USE_BOX2D_PHYSICS
	#include "Box2D/Box2D.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif

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
	osg::Vec3 _velocity;
#ifdef USE_BOX2D_PHYSICS
	b2Body *physicsBody;
#else
	//btCollisionObject* _physicsBody;
	btPairCachingGhostObject* _physicsBody;
#endif
	osg::Vec3 physicsToModelAdjustment;	/// adjustment between the visual and physical components
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

	void checkForCollisions();

	void onUpdate(float deltaTime)
	{
		//_physicsBody->setLinearVelocity(btVector3(1,1,1));
		//getDebugDisplayer()->addText(osgbCollision::asOsgVec3(_physicsBody->getLinearVelocity()));
		//_velocity = osg::Vec3(1.0,1.0,1.0);
		//_physicsBody->getWorldTransform().setOrigin(_physicsBody->getWorldTransform().getOrigin() + osgbCollision::asBtVector3(_velocity * deltaTime));
		/// Bullet does not allow you to set a velocity for kinematic objects, so we need to manually warp the projectile's body. This also means there's no collision detection.
		btTransform transform;
		//_physicsBody->getMotionState()->getWorldTransform(transform);
		transform = _physicsBody->getWorldTransform();
		transform.setOrigin(_physicsBody->getWorldTransform().getOrigin() + osgbCollision::asBtVector3(_velocity * deltaTime));
		//_physicsBody->getMotionState()->setWorldTransform(transform);
		_physicsBody->setWorldTransform(transform);

		checkForCollisions();

	}



protected:
private:
};

void registerProjectileStats();


#endif // PROJECTILE_H
