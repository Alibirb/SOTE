/*
 * input.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: daniel
 */

#include "input.h"
#include "globals.h"


#include "Player.h"
#include "Enemy.h"
#include "Level.h"





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

		if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Escape)
		{
			std::cout << "Detected escape key" << std::endl;
			runCleanup();
			std::cout << "Cleanup finished" << std::endl;
		}

		if(_inputMode != InputMode::Standard)
			return false;	// In non-standard input mode, we still track keystate, but nothing gets triggered.

		if (ea.getKey() == 'r')
		{
			getViewer()->getCameraManipulator()->home(ea, aa);
			getActivePlayer()->resetPosition();
		}
		else if (ea.getKey() == 'p')
		{
			getCurrentLevel()->getDebugDrawer()->setEnabled(!getCurrentLevel()->getDebugDrawer()->getEnabled());
		}
		else if (ea.getKey() == 'o')
		{
			writeOutSceneGraph();
		}
		else if(ea.getKey() == 'e')
			getActivePlayer()->interact();
		if (ea.getKey() == 'j')
			getActivePlayer()->getWeapon()->fire();
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
			getActivePlayer()->jump();
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Shift_L)
			if (getEnemies().size() > 0)
				getActivePlayer()->aimWeapon(getClosestEnemy(getActivePlayer()->getWorldPosition(), getEnemies()));
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


