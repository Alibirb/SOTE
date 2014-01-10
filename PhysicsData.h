#ifndef PHYSICSDATA_H
#define PHYSICSDATA_H


/// userdata to attach to physics bodies so the objects that own them can be altered (e.g. enemies get hurt)
struct PhysicsUserData
{
	std::string ownerType;
	void* owner;
};

enum CollisionCategories {
	HIT_BOX = 0x0002,
	PAIN_SOURCE = 0x0004,
	OBSTACLE = 0x0008,


	ALL = 0xFFFF
};


#endif	//PHYSICSDATA_H
