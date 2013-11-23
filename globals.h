/*
 * globals.h
 *
 *  Created on: Oct 18, 2013
 *      Author: daniel
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <osg/Node>
#include <osg/Group>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osgText/Text>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <math.h>
#include <iostream>

#include "TextDisplay.h"
#include "RandomGenerator.h"



class keyboardEventHandler : public osgGA::GUIEventHandler
{
public:
	std::map<int, bool> keyState;	// OSG uses an enum for keys. Values for 0-9 and a-z are set equal to the character (KEY_A = 'a')

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

};

extern osg::Group* root;
extern osg::Group* lightGroup;
extern keyboardEventHandler* keh;
extern osgViewer::Viewer viewer;
const double pi = 3.14159265358979323846;
extern int windowWidth;
extern int windowHeight;
extern double deltaTime;




double getDeltaTime();



#endif /* GLOBALS_H_ */
