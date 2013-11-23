#ifndef TWODIMENSIONALCAMERAMANIPULATOR_H
#define TWODIMENSIONALCAMERAMANIPULATOR_H

#include <osgGA/CameraManipulator>
#include <osgGA/OrbitManipulator>

#include "globals.h"
#include "BaseCameraManipulator.h"


using namespace osgGA;

class TwoDimensionalCameraManipulator : public BaseCameraManipulator
{
private:
	float currentZoom;
	float scrollAmount = .25;
	float zoomFactor = .1;
	float minZoom = .01;
	float maxZoom = 100.0;
	bool followPlayer;
public:
	TwoDimensionalCameraManipulator()
	{
		this->setHomePosition( osg::Vec3(0.0, 0.0, -1.0), osg::Vec3(0.0, 0.0, 10.0), osg::Vec3(0.0, -1.0, 0.0));
		this->setTransformation(_homeEye, _homeCenter, _homeUp);
		currentZoom = 1.0f;
		updateProjectionMatrix();
	}

	void centerPlayerInView();

	/// Updates the projection matrix. Call this after currentZoom or windowHeight/windowWidth changes.
	void updateProjectionMatrix()
	{
		viewer.getCamera()->setProjectionMatrixAsOrtho2D(-5/currentZoom, 5/currentZoom, -5/ currentZoom * windowHeight/windowWidth, 5 / currentZoom * windowHeight/windowWidth);
	}

	void zoom(float deltaZoom)
	{

		//currentZoom += deltaZoom;
		currentZoom += currentZoom * deltaZoom;

		if (currentZoom < minZoom)	// Prevent from zooming too far in or out.
			currentZoom = minZoom;
		else if (currentZoom > maxZoom)
			currentZoom = maxZoom;
		updateProjectionMatrix();
	}


	bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if(followPlayer)
			this->centerPlayerInView();
	/*
		float dx = 0.0;
		float dy = 0.0;

		if (keh->keyState['w'])
			dy -= scrollAmount;
		else if (keh->keyState['s'])
			dy += scrollAmount;
		if (keh->keyState['a'])
			dx -= scrollAmount;
		else if (keh->keyState['d'])
			dx += scrollAmount;
		translate(dx, dy);
		*/

		return false;
	}

	bool handleMouseScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

		if (keh->keyState[osgGA::GUIEventAdapter::KEY_Control_L] == true)	// If ctrl is held down, scrolling adjusts zoom
		{
			switch (sm)
			{
			case osgGA::GUIEventAdapter::SCROLL_UP:
				zoom(zoomFactor);
				us.requestRedraw();
				return true;
			case osgGA::GUIEventAdapter::SCROLL_DOWN:
				zoom(-zoomFactor);
				us.requestRedraw();
				return true;
			}
		}
		else	// normal operation: scrolling moves camera.
		{
			switch(sm)
			{
			case osgGA::GUIEventAdapter::SCROLL_UP:
				translate(0.0, -scrollAmount);
				us.requestRedraw();
				return true;

			case osgGA::GUIEventAdapter::SCROLL_DOWN:
				translate(0.0, scrollAmount);
				us.requestRedraw();
				return true;

			case osgGA::GUIEventAdapter::SCROLL_LEFT:	// horizontal scrolling currently only implemented on Linux (and possibly OS X).
				translate(-scrollAmount, 0.0);
				us.requestRedraw();
				return true;

			case osgGA::GUIEventAdapter::SCROLL_RIGHT:
				translate(scrollAmount, 0.0);
				us.requestRedraw();
				return true;

			default:
				std::cout << "Unknown scrolling motion detected";
				std::cout << sm;
				return false;
			}
		}
		return false;

	}

	bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		// Backward and forward buttons are now implemented (on Linux, at least)
		switch(ea.getButton())
		{
		case osgGA::GUIEventAdapter::FORWARD_MOUSE_BUTTON :
			zoom(zoomFactor);
			us.requestRedraw();
			return true;
		case osgGA::GUIEventAdapter::BACKWARD_MOUSE_BUTTON :
			zoom(-zoomFactor);
			us.requestRedraw();
			return true;

		case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON :
			osg::Vec3 eye = osg::Vec3(-10.0, 0.0, -0.1);
			eye.normalize();
			this->setTransformation(eye, osg::Vec3(5.0, 5.0, 5.0), osg::Vec3(0.0, -1.0, 0.0));
			us.requestRedraw();
			return true;
		}
		return false;
	}

protected:

};

#endif // TWDIMENSIONALCAMERAMANIPULATOR_H
