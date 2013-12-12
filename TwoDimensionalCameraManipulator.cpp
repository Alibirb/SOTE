


#include "TwoDimensionalCameraManipulator.h"
#include "Player.h"
#include "input.h"

TwoDimensionalCameraManipulator::TwoDimensionalCameraManipulator()
{
	_up = osg::Vec3(0.0, 1.0, 0.0);
	this->setHomePosition( osg::Vec3(0.0, 0.0, 1.0), osg::Vec3(0.0, 0.0, 0.0), _up);
	this->setTransformation(_homeEye, _homeCenter, _homeUp);
	currentZoom = 1.0f;
	followPlayer = true;
	updateProjectionMatrix();
}

void TwoDimensionalCameraManipulator::centerPlayerInView()
{
	setCenter(getActivePlayer()->getCameraTarget());
}

void TwoDimensionalCameraManipulator::updateProjectionMatrix()
{
	viewer.getCamera()->setProjectionMatrixAsOrtho2D(-5/currentZoom, 5/currentZoom, -5/ currentZoom * windowHeight/windowWidth, 5 / currentZoom * windowHeight/windowWidth);
}

void TwoDimensionalCameraManipulator::zoom(float deltaZoom)
{

	//currentZoom += deltaZoom;
	currentZoom += currentZoom * deltaZoom;

	if (currentZoom < minZoom)	// Prevent from zooming too far in or out.
		currentZoom = minZoom;
	else if (currentZoom > maxZoom)
		currentZoom = maxZoom;
	updateProjectionMatrix();
}


bool TwoDimensionalCameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if(followPlayer)
		this->centerPlayerInView();
/*
	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	getTransformation(eye,center,up);

	getDebugDisplayer()->addText("CameraManipulator eye vector: ");
	getDebugDisplayer()->addText(eye);
	getDebugDisplayer()->addText("\n");
	getDebugDisplayer()->addText("CameraManipulator center: ");
	getDebugDisplayer()->addText(center);
	getDebugDisplayer()->addText("\n");
	getDebugDisplayer()->addText("CameraManipulator up vector: ");
	getDebugDisplayer()->addText(up);
	getDebugDisplayer()->addText("\n");


	//osg::Vec3f eye;
	osg::Vec3d dir;
	//osg::Vec3f up;

	viewer.getCamera()->getViewMatrixAsLookAt(eye,dir,up);

	getDebugDisplayer()->addText("Camera eye vector: ");
	getDebugDisplayer()->addText(eye);
	getDebugDisplayer()->addText("\n");
	getDebugDisplayer()->addText("Camera look at position: ");
	getDebugDisplayer()->addText(dir);
	getDebugDisplayer()->addText("\n");
	getDebugDisplayer()->addText("Camera up vector: ");
	getDebugDisplayer()->addText(up);
	getDebugDisplayer()->addText("\n");
*/
	return false;
}

bool TwoDimensionalCameraManipulator::handleMouseScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

	if (getMainEventHandler()->keyState[osgGA::GUIEventAdapter::KEY_Control_L] == true)	// If ctrl is held down, scrolling adjusts zoom
	{
		switch (sm)
		{
		case osgGA::GUIEventAdapter::SCROLL_UP:
			zoom(zoomFactor);
			aa.requestRedraw();
			return true;
		case osgGA::GUIEventAdapter::SCROLL_DOWN:
			zoom(-zoomFactor);
			aa.requestRedraw();
			return true;
		default:
			return false;
		}
	}
	else	// normal operation: scrolling moves camera.
	{
		switch(sm)
		{
		case osgGA::GUIEventAdapter::SCROLL_UP:
			translate(0.0, -scrollAmount);
			aa.requestRedraw();
			return true;

		case osgGA::GUIEventAdapter::SCROLL_DOWN:
			translate(0.0, scrollAmount);
			aa.requestRedraw();
			return true;

		case osgGA::GUIEventAdapter::SCROLL_LEFT:	// horizontal scrolling currently only implemented on Linux (and possibly OS X).
			translate(-scrollAmount, 0.0);
			aa.requestRedraw();
			return true;

		case osgGA::GUIEventAdapter::SCROLL_RIGHT:
			translate(scrollAmount, 0.0);
			aa.requestRedraw();
			return true;

		default:
			std::cout << "Unknown scrolling motion detected";
			std::cout << sm;
			return false;
		}
	}
	return false;

}

bool TwoDimensionalCameraManipulator::handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	// Backward and forward buttons are now implemented (on Linux, at least)
	switch(ea.getButton())
	{
	case osgGA::GUIEventAdapter::FORWARD_MOUSE_BUTTON :
		zoom(zoomFactor);
		aa.requestRedraw();
		return true;
	case osgGA::GUIEventAdapter::BACKWARD_MOUSE_BUTTON :
		zoom(-zoomFactor);
		aa.requestRedraw();
		return true;
	default:
		return false;
	}
	return false;
}

bool TwoDimensionalCameraManipulator::handleMouseDrag(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	if (ea.getButtonMask() == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)	// If the middle mouse button is held down, we're in free-rotate mode.
	{
		int windowCenterX = (ea.getXmin() + ea.getXmax()) / 2.0f;
		int windowCenterY = (ea.getYmin() + ea.getYmax()) / 2.0f;
		float dx = ea.getX() - windowCenterX;
		float dy = ea.getY() - windowCenterY;

		if( dx == 0.f && dy == 0.f ) return false;	// if there's no movement, return now.

		centerMousePointer( ea, aa );

		_azimuth -= dx * .001;
		_zenith -= dy * .001;
		/*if ( _zenith < zenithMin) zenith = zenithMin;
		else if ( zenith > zenithMax) zenith = zenithMax;
		if ( _azimuth >= 2 * pi ) azimuth -= 2 * pi;
		else if ( azimuth < 0 ) azimuth += 2 * pi;
		*/
		setRotation(_azimuth, _zenith);
		//calculateRotation();
		return true;
	}
	return false;
}

