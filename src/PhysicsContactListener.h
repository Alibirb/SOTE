#ifndef PHYSICSCONTACTLISTENER_H
#define PHYSICSCONTACTLISTENER_H

#include "Box2DIntegration.h"
#include "TextDisplay.h"

class PhysicsContactListener : public b2ContactListener
{
public:
	PhysicsContactListener();
	virtual ~PhysicsContactListener();

	void BeginContact(b2Contact* contact);

	void EndContact(b2Contact* contact)
	{
		//handle end event
	}
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		//handle pre-solveevent
	}
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		//handle post-solve event
	}
protected:
private:
};

#endif // PHYSICSCONTACTLISTENER_H
