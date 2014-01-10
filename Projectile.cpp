#include "Projectile.h"
#include "Level.h"
#include "PhysicsNodeCallback.h"
#include "OwnerUpdateCallback.h"

#include "AngelScriptEngine.h"

#ifndef USE_BOX2D_PHYSICS
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif // USE_BOX2D_PHYSICS
#include "PhysicsData.h"

#include "Fighter.h"

#define PROJECTILE_SCRIPT_LOCATION "media/Projectiles/"

ProjectileStats::ProjectileStats(Damages damages, std::string imageFilename)
{
	this->damages = damages;
	this->imageFilename = imageFilename;
}

ProjectileStats::ProjectileStats(const ProjectileStats& other)
{
	this->damages = other.damages;
	this->imageFilename = other.imageFilename;
}

ProjectileStats::ProjectileStats()
{
	this->imageFilename = "";
}

ProjectileStats ProjectileStats::loadPrototype(std::string& prototypeName)
{
	registerProjectileStats();
	getScriptEngine()->runFile(PROJECTILE_SCRIPT_LOCATION + prototypeName + ".as", "ProjectileStats loadStats()");
	//return *((ProjectileStats*) getScriptEngine()->getReturnObject());
	ProjectileStats stats = *( new ProjectileStats(*((ProjectileStats*) getScriptEngine()->getReturnObject())));
	return stats;
}

Projectile::Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, std::string type, std::string team) : Projectile(startingPosition, heading, ProjectileStats::loadPrototype(type), team)
{
}

Projectile::Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, ProjectileStats stats, std::string team)
{
	this->_team = team;

	setStats(stats);

	position = startingPosition;
	heading.normalize();
	this->heading = heading;
	width = .25;
	height = .25;
	sprite = new Sprite(stats.imageFilename, width, height);
	sprite->setUpdateCallback(new OwnerUpdateCallback<Projectile>(this));
	transformNode = new osg::PositionAttitudeTransform();

	transformNode->setPosition(position);
	transformNode->addChild(sprite);
	addToSceneGraph(transformNode);

#ifdef USE_BOX2D_PHYSICS
	box2DToOsgAdjustment = osg::Vec3(0.0, 0.0, 0.0);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x() - box2DToOsgAdjustment.x() , position.y() - box2DToOsgAdjustment.y());
	physicsBody = getCurrentLevel()->getPhysicsWorld()->CreateBody(&bodyDef);

	//b2PolygonShape collisionShape;
	b2CircleShape collisionShape;
	collisionShape.m_radius = width/2;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &collisionShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.filter.categoryBits = CollisionCategories::PAIN_SOURCE;
	fixtureDef.filter.maskBits = CollisionCategories::HIT_BOX;
	physicsBody->CreateFixture(&fixtureDef);
	physicsBody->SetLinearVelocity(toB2Vec2(heading));



	transformNode->setUpdateCallback(new PhysicsNodeCallback(transformNode, physicsBody, box2DToOsgAdjustment));
#else
	physicsToModelAdjustment = osg::Vec3(0, 0, 0);
	btSphereShape* shape = new btSphereShape(width/2);

	btTransform transform = btTransform();
	transform.setIdentity();
	transform.setOrigin(osgbCollision::asBtVector3(position + physicsToModelAdjustment));
	//btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
/*	btDefaultMotionState* motionState = new btDefaultMotionState();
	motionState->setWorldTransform(transform);
	_physicsBody = new btRigidBody(0, motionState, shape);
	_physicsBody->setWorldTransform(transform);
	//getCurrentLevel()->getBulletWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	_physicsBody->setCollisionShape(shape);
	_physicsBody->setCollisionFlags(_physicsBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	_physicsBody->setActivationState(DISABLE_DEACTIVATION);
	//ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
	_physicsBody->setLinearVelocity(osgbCollision::asBtVector3(heading));
	_velocity = heading;

	getCurrentLevel()->getBulletWorld()->addRigidBody(_physicsBody);
	*/

	_velocity = heading;

	_physicsBody = new btPairCachingGhostObject();
	_physicsBody->setWorldTransform(transform);
	getCurrentLevel()->getBulletWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	_physicsBody->setCollisionShape(shape);
	//_physicsBody->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

//	getCurrentLevel()->getBulletWorld()->addCollisionObject(_physicsBody, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
	getCurrentLevel()->getBulletWorld()->addCollisionObject(_physicsBody, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter);
	//getCurrentLevel()->getBulletWorld()->addAction(controller);


	transformNode->setUpdateCallback(new BulletPhysicsNodeCallback(_physicsBody, -physicsToModelAdjustment));
#endif

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Projectile";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	_physicsBody->setUserPointer(userData);
#endif



}

Projectile::~Projectile()
{
#ifdef USE_BOX2D_PHYSICS
	getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
#else
	getCurrentLevel()->getBulletWorld()->removeCollisionObject(_physicsBody);
	delete _physicsBody;
#endif
}

void Projectile::onCollision()
{
	markForRemoval(this, "Projectile");
}

void Projectile::checkForCollisions()	/// NOTE: Detects overlapping bounding boxes. Not what we want, but it works for now.
{
	btManifoldArray   manifoldArray;
	btBroadphasePairArray& pairArray = _physicsBody->getOverlappingPairCache()->getOverlappingPairArray();
	int numPairs = pairArray.size();

	for (int i = 0; i < numPairs; i++)
	{
		manifoldArray.clear();

		const btBroadphasePair& pair = pairArray[i];

		//unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
		btBroadphasePair* collisionPair = getCurrentLevel()->getBulletWorld()->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
		if (!collisionPair)
			continue;

		if (collisionPair->m_algorithm)
			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

		for (int j = 0; j < manifoldArray.size(); j++)
		{
			btPersistentManifold* manifold = manifoldArray[j];

			PhysicsUserData* dataA = (PhysicsUserData*) manifold->getBody0()->getUserPointer();
			PhysicsUserData* dataB = (PhysicsUserData*) manifold->getBody1()->getUserPointer();

			if(!dataA || !dataB)
			{
				logError("Physics object with no userdata.");
				continue;
			}

			if(dataA->ownerType == "Projectile" && (dataB->ownerType == "Enemy" || dataB->ownerType == "Player") )
			{
				((Fighter*)dataB->owner)->onCollision(((Projectile*)dataA->owner));
				((Projectile*)dataA->owner)->onCollision();
			}
			else if((dataA->ownerType == "Enemy" || dataA->ownerType == "Player") && dataB->ownerType == "Projectile")
			{
				((Fighter*)dataA->owner)->onCollision(((Projectile*)dataB->owner));
				((Projectile*)dataB->owner)->onCollision();
			}


			btScalar directionSign = manifold->getBody0() == _physicsBody ? btScalar(-1.0) : btScalar(1.0);
			for (int p = 0; p < manifold->getNumContacts(); p++)
			{
				const btManifoldPoint& pt = manifold->getContactPoint(p);
				if (pt.getDistance() < 0.f)
				{
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& ptB = pt.getPositionWorldOnB();
					const btVector3& normalOnB = pt.m_normalWorldOnB;
					/// work here

				}
			}
		}
	}
}

void Projectile::setPosition(osg::Vec3 position)
{
	this->position = position;
	this->transformNode->setPosition(position);
}

Damages Projectile::getDamages()
{
	return _stats.damages;
}



namespace AngelScriptWrapperFunctions
{
	void ProjectileStatsConstructor(ProjectileStats *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) ProjectileStats();
	}
	void ProjectileStatsInitConstructor(Damages damages, std::string imageFilename, ProjectileStats *self)
	{
		new(self) ProjectileStats(damages, imageFilename);
	}
	void ProjectileStatsCopyConstructor(ProjectileStats& other, ProjectileStats* self)
	{
		new(self) ProjectileStats(other.damages, other.imageFilename);
	}
	void ProjectileStatsDestructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((ProjectileStats*)memory)->~ProjectileStats();
	}

}

using namespace AngelScriptWrapperFunctions;


void registerProjectileStats()
{
	static bool registered = false;
	if(registered)
		return;

	registerDamages();

	getScriptEngine()->registerObjectType("ProjectileStats", sizeof(ProjectileStats), asOBJ_VALUE | GetTypeTraits<ProjectileStats>() );
	getScriptEngine()->registerConstructor("ProjectileStats", "void f(const Damages &in, const string &in)", asFUNCTION(ProjectileStatsInitConstructor));
	getScriptEngine()->registerConstructor("ProjectileStats", "void f()", asFUNCTION(ProjectileStatsConstructor));
	getScriptEngine()->registerDestructor("ProjectileStats", asFUNCTION(ProjectileStatsDestructor));
	getScriptEngine()->registerConstructor("ProjectileStats", "void f(const ProjectileStats &in)", asFUNCTION(ProjectileStatsCopyConstructor));
	getScriptEngine()->registerObjectProperty("ProjectileStats", "Damages damages", asOFFSET(ProjectileStats, damages));
	getScriptEngine()->registerObjectProperty("ProjectileStats", "string imageFilename", asOFFSET(ProjectileStats, imageFilename));

	//getScriptEngine()->registerObjectMethod("ProjectileStats", "ProjectileStats opAssign(const ProjectileStats &in) const", asMETHODPR(ProjectileStats, operator=, (const ProjectileStats &), ProjectileStats&), asCALL_THISCALL);

	getScriptEngine()->registerFunction("ProjectileStats loadProjectilePrototype(const string &in)", asFUNCTION(ProjectileStats::loadPrototype));

	registered = true;
}
