#include "PhysicsContactListener.h"

#include "Enemy.h"
#include "Projectile.h"

PhysicsContactListener::PhysicsContactListener()
{
	//ctor
}

PhysicsContactListener::~PhysicsContactListener()
{
	//dtor
}

void PhysicsContactListener::BeginContact(b2Contact* contact)
{
	//handle begin event
	Box2DUserData *dataA = (Box2DUserData*) contact->GetFixtureA()->GetBody()->GetUserData();
	Box2DUserData *dataB = (Box2DUserData*) contact->GetFixtureB()->GetBody()->GetUserData();

	if(dataA->ownerType == "Projectile" && dataB->ownerType == "Enemy")
	{
		((Enemy*)dataB->owner)->takeDamage(10.0);
		((Projectile*)dataA->owner)->onCollision();
	}
	else if(dataA->ownerType == "Enemy" && dataB->ownerType == "Projectile")
	{
		((Enemy*)dataA->owner)->takeDamage(10.0);
		((Projectile*)dataB->owner)->onCollision();
	}
}
