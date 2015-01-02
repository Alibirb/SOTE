#include "TransformTool.h"

#include "Editor/Editor.h"
#include "Editor/EditorCameraManipulator.h"

#include "GameObject.h"

#include "Level.h"



TransformTool::TransformTool() : _mode(NO_MODE), _lastMouseX(-1), _lastMouseY(-1)
{
	_name = "Transform tool";
	_description = "Basic transform tools. We're using Unity's keybindings, so press 'w' for translation, 'e' for rotation, and 'r' to scale.";

	_activationKeys.push_back('w');
	_activationKeys.push_back('e');
	_activationKeys.push_back('r');

	_transformCenterNode = new ImprovedMatrixTransform();
	_transformNode = new ImprovedMatrixTransform();
	_fakeOrigin = new ImprovedMatrixTransform();

	_transformCenterNode->addChild(_transformNode);
	_transformNode->addChild(_fakeOrigin);
	addToSceneGraph(_transformCenterNode);
}

TransformTool::~TransformTool()
{
	//dtor
}


void TransformTool::onToolSelected()
{
	_lastMouseX = -1;
	_lastMouseY = -1;
}
void TransformTool::onToolDeselected()
{
	_lastMouseX = -1;
	_lastMouseY = -1;
	_constraintMode = NOT_CONSTRAINED;
	_mode = NO_MODE;
}


void TransformTool::beginTransform()
{
	_transformCenter = getEditor()->getSelectedObjects().front()->getWorldPosition();

	_transformCenterOnScreen = _transformCenter
							* getViewer()->getCamera()->getViewMatrix()
							* getViewer()->getCamera()->getProjectionMatrix();
	_transformCenterOnScreen *= windowHeight;
	_transformCenterOnScreen = osg::Vec3(_transformCenterOnScreen.x() + windowWidth/2, _transformCenterOnScreen.y() + windowHeight/2, 0);


	_transformCenterNode->makeIdentity();
	_transformNode->makeIdentity();
	_fakeOrigin->makeIdentity();

	_transformCenterNode->setPosition(_transformCenter);
	_fakeOrigin->setPosition(-_transformCenter);

	for(GameObject* obj : getEditor()->getSelectedObjects())
	{
		obj->parentTo(_fakeOrigin);
	}
}

void TransformTool::cancelTransform()
{
	for(GameObject* obj : getEditor()->getSelectedObjects())
	{
		obj->parentTo(getCurrentLevel()->getRootNode());
	}
	getEditor()->setCurrentTool(NULL);
}

void TransformTool::applyTransform()
{
	/// Construct a matrix for the total transform. Note that OSG uses post multiplication, so we seem to be applying things backwards. It's weird.
	osg::Matrix transformation =  _fakeOrigin->getMatrix() * _transformNode->getMatrix() * _transformCenterNode->getMatrix();

	for(GameObject* obj : getEditor()->getSelectedObjects())
	{
		obj->getTransformNode()->postMult(transformation);

		osg::Matrix newObjectTransform = obj->getTransformNode()->getMatrix();	/// debugging

		obj->parentTo(getCurrentLevel()->getRootNode());
		obj->recalculatePhysicsTransform();

		newObjectTransform = obj->getTransformNode()->getMatrix();
	}
}


void TransformTool::onMouseButtonPush(Event& event)
{
	if(event.getButton() == Event::LEFT_MOUSE_BUTTON)
	{
		applyTransform();

		getEditor()->setCurrentTool(NULL);
	}
}

void TransformTool::onMouseMove(Event& event)
{
	const osgGA::GUIEventAdapter* ea = event.getOsgEvent();

	if(_lastMouseX < 0 || _lastMouseY < 0)
	{
		/// Set the base mouse position.
		_lastMouseX = ea->getX();
		_lastMouseY = ea->getY();

		return;
	}

	float dx = ea->getX() - _lastMouseX;
	float dy = ea->getY() - _lastMouseY;

	/// If the difference is less than one pixel, ignore it.
	if(dx < 1 && dx > -1)
		dx = 0;
	if(dy < 1 && dy > -1)
		dy = 0;

	if( dx == 0.f && dy == 0.f )
		return;	/// if there's no movement, return now.


	switch(_mode)
	{
	case TRANSLATE:
	{
		osg::Vec3 dv = getEditor()->getCameraManipulator()->screenToWorldTranslation(_transformCenter, ea->getX() - _startingMouseX, ea->getY() - _startingMouseY);

		switch(_constraintMode)
		{
		case CONSTRAIN_TO_X_AXIS:
			dv = osg::Vec3(dv.x(), 0, 0);
			break;
		case CONSTRAIN_TO_Y_AXIS:
			dv = osg::Vec3(0, dv.y(), 0);
			break;
		case CONSTRAIN_TO_Z_AXIS:
			dv = osg::Vec3(0, 0, dv.z());
			break;
		}

		_transformNode->setPosition(dv);

		break;
	}
	case ROTATE:
	{
		osg::Vec3 axis;

		switch(_constraintMode)
		{
		case CONSTRAIN_TO_X_AXIS:
			axis = osg::Vec3(1, 0, 0);
			break;
		case CONSTRAIN_TO_Y_AXIS:
			axis = osg::Vec3(0, 1, 0);
			break;
		case CONSTRAIN_TO_Z_AXIS:
			axis = osg::Vec3(0, 0, 1);
			break;
		default:
			axis = getEditor()->getCameraManipulator()->getMatrix().getTrans();
			axis.normalize();
			break;
		}

		osg::Quat rotation;

		double angle;

	/*	osg::Vec3 _transformCenterOnScreen;
		_transformCenterOnScreen = _transformCenter
								* getViewer()->getCamera()->getViewMatrix()
								* getViewer()->getCamera()->getProjectionMatrix();

		_transformCenterOnScreen *= windowHeight;
		_transformCenterOnScreen = osg::Vec3(_transformCenterOnScreen.x() + windowWidth/2, _transformCenterOnScreen.y() + windowHeight/2, 0);*/

		osg::Vec2 u = osg::Vec2(_startingMouseX - _transformCenterOnScreen.x(), _startingMouseY - _transformCenterOnScreen.y());
		osg::Vec2 v = osg::Vec2(ea->getX() - _transformCenterOnScreen.x(), ea->getY() - _transformCenterOnScreen.y());
		u.normalize();
		v.normalize();

		double angleU = acos(u.x());
		if(u.y() < 0)
			angleU = -angleU;

		double angleV = acos(v.x());
		if(v.y() < 0)
			angleV = -angleV;

		angle = angleV - angleU;

		rotation.makeRotate(angle, axis);

		_transformNode->setAttitude(rotation);

		break;
	}

	case SCALE:
	{
		osg::Vec3 dv;
		double initialDistance = sqrt( pow(_startingMouseX - _transformCenterOnScreen.x(), 2) + pow(_startingMouseY - _transformCenterOnScreen.y(), 2) );
		double distance = sqrt( pow(ea->getX() - _transformCenterOnScreen.x(), 2) + pow(ea->getY() - _transformCenterOnScreen.y(), 2) );
		double scale = distance / initialDistance;

		switch(_constraintMode)
		{
		case CONSTRAIN_TO_X_AXIS:
			dv = osg::Vec3(scale, 0, 0);
			break;
		case CONSTRAIN_TO_Y_AXIS:
			dv = osg::Vec3(0, scale, 0);
			break;
		case CONSTRAIN_TO_Z_AXIS:
			dv = osg::Vec3(0, 0, scale);
			break;
		default:
			dv = osg::Vec3(scale, scale, scale);
			break;
		}

		_transformNode->setScale(dv);

		break;

	}

	}

	/// Update the last mouse position for next time.
	_lastMouseX = ea->getX();
	_lastMouseY = ea->getY();

}

void TransformTool::onKeyDown(Event& event)
{
	switch(_mode)
	{
	case NO_MODE:
	{
		switch(event.getKey())
		{
		case 'w':
			_mode = TRANSLATE;
			_startingMouseX = event.getOsgEvent()->getX();
			_startingMouseY = event.getOsgEvent()->getY();
			_lastMouseX = _startingMouseX;
			_lastMouseY = _startingMouseY;
			beginTransform();
			break;

		case 'e':
			_mode = ROTATE;
			_startingMouseX = event.getOsgEvent()->getX();
			_startingMouseY = event.getOsgEvent()->getY();
			_lastMouseX = _startingMouseX;
			_lastMouseY = _startingMouseY;
			beginTransform();
			break;

		case 'r':
			_mode = SCALE;
			_startingMouseX = event.getOsgEvent()->getX();
			_startingMouseY = event.getOsgEvent()->getY();
			_lastMouseX = _startingMouseX;
			_lastMouseY = _startingMouseY;
			beginTransform();
			break;
		}
		break;
	}
	default:
	{
		switch(event.getKey())
		{
		case 'x':
			/// Constrain to X axis
			if(_constraintMode != CONSTRAIN_TO_X_AXIS)
				_constraintMode = CONSTRAIN_TO_X_AXIS;
			else
				_constraintMode = NOT_CONSTRAINED;
			break;
		case 'y':
			/// Constrain to Y axis
			if(_constraintMode != CONSTRAIN_TO_Y_AXIS)
				_constraintMode = CONSTRAIN_TO_Y_AXIS;
			else
				_constraintMode = NOT_CONSTRAINED;
			break;
		case 'z':
			/// Constrain to Z axis
			if(_constraintMode != CONSTRAIN_TO_Z_AXIS)
				_constraintMode = CONSTRAIN_TO_Z_AXIS;
			else
				_constraintMode = NOT_CONSTRAINED;
			break;

		case Event::KEY_Escape:
			cancelTransform();
			break;
		}
		break;
	}

	}

}





