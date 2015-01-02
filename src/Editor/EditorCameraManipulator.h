#ifndef EDITORCAMERAMANIPULATOR_H
#define EDITORCAMERAMANIPULATOR_H

#include "BaseCameraManipulator.h"


/// Camera manipulator used in Edit mode.
class EditorCameraManipulator : public BaseCameraManipulator
{
protected:
	float _zenithMin;
	float _zenithMax;
	float _zoomFactor = .1;
	float _minDistance;

	float _lastMouseX, _lastMouseY;

public:
	EditorCameraManipulator();
	virtual ~EditorCameraManipulator();

	void zoom(float deltaZoom);

	void updateProjectionMatrix();

	/** Converts a change in screen coordinates (pixels) to the equivalent world coordinate translation.
	 * Must pass in the current location of the point you want transformed.
	 */
	osg::Vec3 screenToWorldTranslation(osg::Vec3 point, double dx, double dy, double dz=0);


	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:
	bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
//	bool handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	//bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	bool handleMouseScroll(const GUIEventAdapter& ea, GUIActionAdapter& aa);
	bool handleKeyDown(const GUIEventAdapter& ea, GUIActionAdapter& aa);

	void calculateRotation();
};

#endif // EDITORCAMERAMANIPULATOR_H
