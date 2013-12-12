/*
 * input.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: daniel
 */

#include "input.h"
#include "globals.h"
#include "Player.h"

bool MainEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
	{
		if(_inputMode == InputMode::Standard)
		{
			static int moveUpKey = 'w';
			static int moveDownKey = 's';
			static int moveLeftKey = 'a';
			static int moveRightKey = 'd';

			osg::Vec3 movementVector = osg::Vec3(0,0,0);
			if (keyState[moveUpKey]) movementVector += osg::Vec3(0, 1, 0);
			if (keyState[moveDownKey]) movementVector += osg::Vec3(0, -1, 0);
			if (keyState[moveLeftKey]) movementVector += osg::Vec3(-1, 0, 0);
			if (keyState[moveRightKey]) movementVector += osg::Vec3(1, 0, 0);
			movementVector.normalize();
			getActivePlayer()->processMovementControls(movementVector);
		}
		else
		{
			getActivePlayer()->processMovementControls(osg::Vec3(0,0,0));	// Send empty controls to player. FIXME: ugly.

		}
	}
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		keyState[ea.getKey()] = true;

		if(_inputMode != InputMode::Standard)
			return false;	// In non-standard input mode, we track keystate, but don't act on anything.

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

MainEventHandler* getMainEventHandler()
{
	static MainEventHandler* handler = new MainEventHandler();
	return handler;
}


