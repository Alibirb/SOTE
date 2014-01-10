/*
 * ThirdPersonCameraManipulator.h
 *
 *  Created on: Aug 9, 2013
 *      Author: daniel
 */

#ifndef THIRD_PERSON_CAMERA_MANIPULATOR_H_
#define THIRD_PERSON_CAMERA_MANIPULATOR_H_

#include "BaseCameraManipulator.h"
#include <osg/Quat>
#include "Player.h"

using namespace osgGA;



class ThirdPersonCameraManipulator : public BaseCameraManipulator
{
protected:
	float _zenithMin;
	float _zenithMax;
	float _zoomFactor = .1;
	float _minDistance;
	float _maxDistance;

public:
	ThirdPersonCameraManipulator();

	void centerPlayerInView()
	{
		setCenter(getActivePlayer()->getCameraTarget());
	}



	void zoom(float deltaZoom);

	void updateProjectionMatrix();

	bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		centerPlayerInView();
		return false;
	}

	bool handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	bool handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	bool handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	bool handleMouseScroll( const GUIEventAdapter& ea, GUIActionAdapter& aa );

	void calculateRotation()
	{
		_rotation =  osg::Quat(_zenith, osg::Vec3(1,0,0)) * osg::Quat(_azimuth, osg::Vec3(0,0,1));
	}

};


#endif /* THIRD_PERSON_CAMERA_MANIPULATOR_H_ */


