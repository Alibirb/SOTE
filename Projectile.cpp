#include "Projectile.h"
#include "Level.h"
#include "PhysicsNodeCallback.h"

#include "AngelScriptEngine.h"

#ifndef USE_BOX2D_PHYSICS
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif // USE_BOX2D_PHYSICS
#include "PhysicsData.h"

#include "Fighter.h"

#include "GameObjectData.h"

#define PROJECTILE_SCRIPT_LOCATION "media/Projectiles/"





Projectile::Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, GameObjectData* dataObj, std::string team)
{
	_objectType = "Projectile";
	this->_team = team;

	load(dataObj);

	osg::Vec3 position = startingPosition;
	heading.normalize();
	this->heading = heading;

	width = .25;
	height = .25;
	//setModelNode(new Sprite(stats.imageFilename, width, height));

#ifdef USE_BOX2D_PHYSICS
	box2DToOsgAdjustment = osg::Vec3(0.0, 0.0, 0.0);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x() - box2DToOsgAdjustment.x() , position.y() - box2DToOsgAdjustment.y());
	physicsBody = getCurrentLevel()->getPhysicsWorld()->CreateBody(&bodyDef);

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

	_velocity = heading;

	_physicsBody = new btPairCachingGhostObject();
	_physicsBody->setWorldTransform(transform);
	_physicsBody->setCollisionShape(shape);
	getCurrentLevel()->getBulletWorld()->addCollisionObject(_physicsBody, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter);

	_transformNode->setUpdateCallback(new BulletPhysicsNodeCallback(_physicsBody, -physicsToModelAdjustment));
#endif

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Projectile";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	_physicsBody->setUserPointer(userData);
#endif

	setPosition(startingPosition);
}


Projectile::~Projectile()
{
}

Damages Projectile::getDamages()
{
	return _damages;
}



std::string Projectile::getTeam()
{
	return _team;
}

void Projectile::onUpdate(float deltaTime)
{
	/// Bullet does not allow you to set a velocity for kinematic objects, so we need to manually warp the projectile's body. This also means there's no collision detection.
	btTransform transform;
	transform = _physicsBody->getWorldTransform();
	transform.setOrigin(_physicsBody->getWorldTransform().getOrigin() + osgbCollision::asBtVector3(_velocity * deltaTime));
	_physicsBody->setWorldTransform(transform);
}
void Projectile::onCollision(GameObject* other)
{
	if(dynamic_cast<Fighter*>(other))
	{
		markForRemoval(this, "Projectile");
		//_stats.damages.clear();	/// Possible way to ensure the damage is only dealt once. Could cause no damage to be dealt, however, if the Projectile's onCollision is called before that of the Fighter.
	}

}

GameObjectData* Projectile::save()
{
	GameObjectData* dataObj =  new GameObjectData(_objectType);
	saveGameObjectVariables(dataObj);
	saveProjectileData(dataObj);
	return dataObj;
}
void Projectile::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadProjectileData(dataObj);
}


void Projectile::saveProjectileData(GameObjectData* dataObj)
{
	std::vector<GameObjectData*> damageDataVector;
	for(Damage damage : _damages)
	{
		GameObjectData* damageData = new GameObjectData("damage");
		damageData->addData("type", damage.type);
		damageData->addData("amount", damage.amount);
		//dataObj->addChild(damageData);
		damageDataVector.push_back(damageData);
	}
	dataObj->addData("damages", damageDataVector);

}
void Projectile::loadProjectileData(GameObjectData* dataObj)
{
	//for(GameObjectData* child : dataObj->getChildren())
	for(GameObjectData* child : dataObj->getObjectList("damages"))
	{
		if(child->getType() == "damage")
		{
			Damage dam;
			dam.type = child->getString("type");
			dam.amount = child->getFloat("amount");
			_damages.push_back(dam);
		}
	}
}


/*
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
*/
