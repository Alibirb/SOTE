/*
 * Entity.h
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include "globals.h"
#include "Sprite.h"



class Entity
{
public:
	std::string name;
	osg::Vec3 position;
	osg::Vec3 initialPosition;

	osg::Geode *modelNode;
	osg::PositionAttitudeTransform *transformNode;
	float maxSpeed = 3.0f;


	enum State {
		awake, napping, dead
	};

	State state;

	Entity(std::string name, osg::Vec3 position);

	virtual void jump();

	virtual void resetPosition() {
		this->setPosition(initialPosition);
	}

	virtual ~Entity();

	virtual osg::Vec3 getPosition() {
		return transformNode->getPosition();
	}
	virtual void setPosition(osg::Vec3 newPosition) {
		transformNode->setPosition(newPosition);
	}
/*
	double getHeading()
	{
		double angle;
		osg::Vec3 axis;
		transformNode->getAttitude().getRotate(angle, axis);
		if (axis == osg::Vec3(0,0,1))	// make sure the rotation is only around the z-axis. If it's not, this will crash the program, because nothing is returned.
		{
			return angle;
		}
		//FIXME: this will crash if the rotation isn't centered on the z-axis.
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
	}
*/
};

void removeDeadEntities();
void markEntityForRemoval(Entity *toBeEXTERMINATED);

#endif /* ENTITY_H_ */
