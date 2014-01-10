/*
 * Entity.h
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <osg/PositionAttitudeTransform>

#include "globals.h"

#ifdef USE_BOX2D_PHYSICS
	#include "Box2DIntegration.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
	#include "BulletDynamics/Character/btKinematicCharacterController.h"
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif




/// Class for characters
/// TODO: Should probably be called "Character".
class Entity
{
protected:
	bool _useSpriteAsModel = false;

public:
	std::string name;
	osg::Vec3 position;
	osg::Vec3 initialPosition;

	osg::ref_ptr<osg::Node> modelNode;
	osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;
	//float maxSpeed = 3.0f;
	float maxSpeed = 6.0f;
#ifdef USE_BOX2D_PHYSICS
	b2Body* physicsBody;
#else
	btKinematicCharacterController* controller;
#endif
	osg::Vec3 physicsToModelAdjustment;	/// adjustment between the visual and physical components


	enum State {
		awake, napping, dead
	};

	State state;

	Entity(std::string name, osg::Vec3 position);

	void loadModel(std::string modelFilename);

	virtual void jump();

	virtual void resetPosition() {
		this->setPosition(initialPosition);
	}

	virtual ~Entity();

	virtual osg::Vec3 getPosition() {
		return transformNode->getPosition();
	}
	virtual osg::Vec3 getWorldPosition()
	{
		return getWorldCoordinates(transformNode)->getTrans();
	}
	virtual void setPosition(osg::Vec3 newPosition) {
		transformNode->setPosition(newPosition);
#ifdef USE_BOX2D_PHYSICS
		physicsBody->SetTransform(toB2Vec2(newPosition - box2DToOsgAdjustment), physicsBody->GetAngle());
#else
		controller->warp(osgbCollision::asBtVector3(newPosition - physicsToModelAdjustment));
		controller->setVelocityForTimeInterval(btVector3(0,0,0), 0.0);
#endif
	}

	double getHeading()
	{
		double angle;
		osg::Vec3 axis;
		transformNode->getAttitude().getRotate(angle, axis);
		if (axis == osg::Vec3(0,0,1))	// make sure the rotation is only around the z-axis. If it's not, this will crash the program, because nothing is returned.
		{
			return angle;
		}
		else if (axis == osg::Vec3(0,0,-1))
			return -angle;
		logError("Can only obtain heading for entities rotated around z-axis.\n");
		//FIXME: this will not work if the rotation isn't centered on the z-axis.
	}
	void setHeading(double angle)
	{
		this->setAttitude(osg::Quat(angle, osg::Vec3(0,0,1)));
	}

	const osg::Quat& getAttitude()
	{
		return transformNode->getAttitude();
	}
	void setAttitude(const osg::Quat& newAttitude)
	{
		transformNode->setAttitude(newAttitude);
#ifndef USE_BOX2D_PHYSICS
		controller->getGhostObject()->getWorldTransform().setBasis(osgbCollision::asBtMatrix3x3(osg::Matrix(newAttitude)));
#endif
	}

	virtual void onUpdate(float deltaTime)=0;
};



#endif /* ENTITY_H_ */
