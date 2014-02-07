#include "Projectile.h"
#include "Level.h"
#include "PhysicsNodeCallback.h"

#include "AngelScriptEngine.h"

#ifndef USE_BOX2D_PHYSICS
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif // USE_BOX2D_PHYSICS
#include "PhysicsData.h"

#include "Fighter.h"

#define PROJECTILE_SCRIPT_LOCATION "media/Projectiles/"

#include "tinyxml/tinyxml2.h"


ProjectileStats::ProjectileStats(XMLElement* xmlElement)
{
	load(xmlElement);
}

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

void ProjectileStats::load(XMLElement* xmlElement)
{
	if(xmlElement->Attribute("source"))		/// Load from external source first, then apply changes.
		load(xmlElement->Attribute("source"));


	XMLElement* currentElement = xmlElement->FirstChildElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			imageFilename = currentElement->Attribute("source");
		else if(elementType == "damage")
		{
			Damage dam = Damage();
			//std::string tddsd = currentElement->Attribute("type");
			dam.type = currentElement->Attribute("type");
			currentElement->QueryFloatAttribute("amount", &dam.amount);
			damages.push_back(dam);
		}

	}
}

void ProjectileStats::load(std::string xmlFilename)
{
	FILE *file = fopen(xmlFilename.c_str(), "rb");
	if(!file)
	{
		xmlFilename = PROJECTILE_SCRIPT_LOCATION + xmlFilename;
		file = fopen(xmlFilename.c_str(), "rb");
		if(!file)
			logError("Failed to open file " + xmlFilename);
	}


	XMLDocument doc(xmlFilename.c_str());
	if (doc.LoadFile(file)  != tinyxml2::XML_NO_ERROR)
	{
		logError("Failed to load file " + xmlFilename);
		logError(doc.GetErrorStr1());
	}


	XMLHandle docHandle(&doc);
	XMLElement* rootElement = docHandle.FirstChildElement().ToElement();

	load(rootElement);
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

	osg::Vec3 position = startingPosition;
	heading.normalize();
	this->heading = heading;

	width = .25;
	height = .25;
	setModelNode(new Sprite(stats.imageFilename, width, height));

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
	return _stats.damages;
}

void Projectile::setStats(ProjectileStats& stats)
{
	this->_stats = stats;
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

//	checkForCollisions();
}
void Projectile::onCollision(GameObject* other)
{
	if(dynamic_cast<Fighter*>(other))
	{
		markForRemoval(this, "Projectile");
		//_stats.damages.clear();	/// Possible way to ensure the damage is only dealt once. Could cause no damage to be dealt, however, if the Projectile's onCollision is called before that of the Fighter.
	}

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
