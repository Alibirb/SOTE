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

	if(!dataA || !dataB)
	{
		logError("Physics object with no userdata.");
		return;
	}

	if(dataA->ownerType == "Projectile" && (dataB->ownerType == "Enemy" || dataB->ownerType == "Player") )
	{
		((Fighter*)dataB->owner)->takeDamage(((Projectile*)dataA->owner)->getDamage());
		((Projectile*)dataA->owner)->onCollision();
	}
	else if((dataA->ownerType == "Enemy" || dataA->ownerType == "Player") && dataB->ownerType == "Projectile")
	{
		((Fighter*)dataA->owner)->takeDamage(((Projectile*)dataB->owner)->getDamage());
		((Projectile*)dataB->owner)->onCollision();
	}
}
