#include "Projectile.h"
#include "Level2D.h"



Projectile::Projectile(osg::Vec3 startingPosition, osg::Vec3 heading)
{
	position = startingPosition;
	this->heading = heading;
	sprite = new Sprite(DEFAULT_PROJECTILE_IMAGE);
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->setPosition(position);
	transformNode->addChild(sprite);
	root->addChild(transformNode);
	transformNode->setUpdateCallback(new ProjectileNodeCallback(this));

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x(), position.y());
	physicsBody = getCurrentLevel()->getPhysicsWorld()->CreateBody(&bodyDef);
	b2PolygonShape collisionBox;
	collisionBox.SetAsBox(1.0f, 1.0f);
	physicsBody->CreateFixture(&collisionBox, 0.0f);
}

Projectile::~Projectile()
{
	//dtor
}

void Projectile::setPosition(osg::Vec3 position)
{
	this->position = position;
}

void Projectile::advance()
{
	//position += heading *.01;
	transformNode->setPosition(osg::Vec3(physicsBody->GetPosition().x, physicsBody->GetPosition().y, transformNode->getPosition().z()));
}


void ProjectileNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	projectile->advance();

	traverse(node, nv);	// need to call this so scene graph traversal continues.
}
