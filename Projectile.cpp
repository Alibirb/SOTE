#include "Projectile.h"
#include "Level2D.h"
#include "PhysicsNodeCallback.h"


Projectile::Projectile(osg::Vec3 startingPosition, osg::Vec3 heading)
{
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

	//box2DToOsgAdjustment = osg::Vec3(-width/2, -height/2, 0.0);
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

float Projectile::getDamage()
{
	return _damage;
}
