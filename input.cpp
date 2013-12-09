/*
 * input.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: daniel
 */

#include "globals.h"
#include "Player.h"

bool keyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		keyState[ea.getKey()] = true;
		if (ea.getKey() == 'r')
		{
			getActivePlayer()->resetPosition();
			Enemy *enemy = new Enemy("theEnemy", osg::Vec3(5.0, 5.0, 0.0));
		}
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Return)
			if (getEnemies().size() > 0)
				getActivePlayer()->attack(getEnemies().front());
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
			if (getEnemies().size() > 0)
				getActivePlayer()->aimWeapon(getEnemies().front());
		return false;
	}
	case(osgGA::GUIEventAdapter::KEYUP):
	{
		keyState[ea.getKey()] = false;
		return false;
	}
	default:
		return false;
	}
}


