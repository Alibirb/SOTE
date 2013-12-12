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
#include <osgViewer/Viewer>

#include <math.h>
#include <iostream>


// NOTE: Anything included here needs to have minimal dependencies, so it won't affect compilation time too much.
#include "TextDisplay.h"
#include "RandomGenerator.h"
#include "ExpiredObjectRemoval.h"






extern osg::Group* root;
extern osg::Group* lightGroup;
extern osgViewer::Viewer viewer;
const double pi = 3.14159265358979323846;
extern int windowWidth;
extern int windowHeight;
extern double deltaTime;

void logError(std::string errorMessage);
void logWarning(std::string warning);

double sind(double x);
double cosd(double x);
double tand(double x);
double asind(double x);
double acosd(double x);
double atand(double x);


osg::Vec3 getWorldCoordinates(osg::Node *node);

double getDeltaTime();



#endif /* GLOBALS_H_ */
