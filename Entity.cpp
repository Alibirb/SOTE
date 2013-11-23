/*
 * Entity.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#include "Entity.h"
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

std::deque<Entity*> deadEntities;

Entity::Entity(std::string name, osg::Vec3 position)
{
	initialPosition = position;

	this->name = name;
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->setPosition(position);

	modelNode = new Sprite();
	root->addChild(transformNode);
	transformNode->addChild(modelNode);


	state = awake;
}

Entity::~Entity()
{
	transformNode->getParent(0)->removeChild(transformNode);	// remove the node from the scenegraph.
}

void Entity::jump()
{

}

void removeDeadEntities()
{
	while (deadEntities.size() > 0)
	{
		delete deadEntities.front();
		deadEntities.pop_front();
	}
}

void markEntityForRemoval(Entity *toBeEXTERMINATED) {
	deadEntities.push_back(toBeEXTERMINATED);
}
