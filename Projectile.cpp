#include "Projectile.h"
#include "Level2D.h"
#include "PhysicsNodeCallback.h"

#include "AngelScriptEngine.h"


ProjectileStats::ProjectileStats(Damages damages, std::string imageFilename)
{
	this->damages = damages;
	this->imageFilename = imageFilename;
}

ProjectileStats::ProjectileStats()
{

}


Projectile::Projectile(osg::Vec3 startingPosition, osg::Vec3 heading, std::string type)
{
	loadStats("media/Projectiles/" + type + ".as");


	position = startingPosition;
	heading.normalize();
	this->heading = heading;
	width = .25;
	height = .25;
	sprite = new Sprite(DEFAULT_PROJECTILE_IMAGE, width, height);
	transformNode = new osg::PositionAttitudeTransform();

	transformNode->setPosition(position);
	transformNode->addChild(sprite);
	addToSceneGraph(transformNode);

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
	Box2DUserData *userData = new Box2DUserData;
	userData->owner = this;
	userData->ownerType = "Projectile";
	physicsBody->SetUserData(userData);


	transformNode->setUpdateCallback(new PhysicsNodeCallback(transformNode, physicsBody, box2DToOsgAdjustment));

}

Projectile::~Projectile()
{
	getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
}

void Projectile::onCollision()
{
	markForRemoval(this, "Projectile");
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

void Projectile::loadStats(std::string scriptFilename)
{
	registerProjectileStats();
	getScriptEngine()->runFile(scriptFilename, "ProjectileStats loadStats()");
	setStats( *((ProjectileStats*) getScriptEngine()->getReturnObject()));
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

	registered = true;
}
