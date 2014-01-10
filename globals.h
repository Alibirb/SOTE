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


using namespace osg;

//extern osg::ref_ptr<osg::Group> root;
//extern ref_ptr<osg::Group> lightGroup;
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

osg::Vec3 cameraToWorldTranslation(float x, float y, float z = 0);
osg::Vec3 cameraToWorldTranslation(osg::Vec3 camTranslation);


osg::Matrixd* getWorldCoordinates( osg::Node* node);

float getDistance(osg::Vec3 a, osg::Vec3 b);

double getDeltaTime();

/// Called when the game is lost.
void GameOverYouLose();

void runCleanup();	/// Cleans up before the program exits.

/// Adds a node to the graph as a child of the specified parent, if it's safe to do so (not during a scene graph traversal).
/// If it's not safe, puts the node on a list of additions, and adds it once it's safe.
void addToSceneGraph(Node* node, Group* parent=root);



#endif /* GLOBALS_H_ */
