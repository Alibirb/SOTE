#ifndef TWODIMENSIONALCAMERAMANIPULATOR_H
#define TWODIMENSIONALCAMERAMANIPULATOR_H


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
	bool ThreeDRotationOn;
public:
	TwoDimensionalCameraManipulator();

	void centerPlayerInView();

	/// Updates the projection matrix. Call this after currentZoom or windowHeight/windowWidth changes.
	void updateProjectionMatrix();

	void zoom(float deltaZoom);

	bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool handleMouseScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool handleMouseDrag(const GUIEventAdapter& ea, GUIActionAdapter& aa);

protected:

};

#endif // TWDIMENSIONALCAMERAMANIPULATOR_H
