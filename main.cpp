#include <iostream>

#include "globals.h"
#include "Level2D.h"
#include "Player.h"
#include "Enemy.h"
#include "TwoDimensionalCameraManipulator.h"
#include "AngelScriptEngine.h"
#include "AngelScriptConsole.h"


#include <osgViewer/config/SingleWindow>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

using namespace std;


/// Variables declared extern in globals.h
osg::Group* root;
osg::Group* lightGroup;
osgViewer::Viewer viewer;
int windowWidth, windowHeight;
double deltaTime;

void logError(std::string errorMessage)
{
	std::cout << errorMessage << std::endl;
}
void logWarning(std::string warning)
{
	std::cout << warning << std::endl;
}

double getDeltaTime()
{
	return deltaTime;
}

double sind(double x) {
	return sin(x*pi/180);
}
double cosd(double x) {
	return cos(x*pi/180);
}
double tand(double x) {
	return tan(x*pi/180);
}
double asind(double x) {
	return asin(x) * 180/pi;
}
double acosd(double x) {
	return acos(x) * 180/pi;
}
double atand(double x) {
	return atan(x) * 180/pi;
}

/*
osg::Vec3 getWorldCoordinates(osg::Node *node)
{

	osg::Vec3 localPosition;
	if(node->asTransform() != 0)
		if(node->asTransform()->asPositionAttitudeTransform() != 0)
			localPosition = node->asTransform()->asPositionAttitudeTransform()->getPosition();
	if (node->getNumParents() == 0)
		return localPosition;	// No parents, which means this is the root node. Or it's not attached to the scene graph. In either case, this is the last node we look at.
	else
		return localPosition + getWorldCoordinates(node->getParent(0));	// Add on the position of the parent node.


}
*/



// Visitor to return the world coordinates of a node.
// It traverses from the starting node to the parent.
// The first time it reaches a root node, it stores the world coordinates of
// the node it started from.  The world coordinates are found by concatenating all
// the matrix transforms found on the path from the start node to the root node.

class getWorldCoordOfNodeVisitor : public osg::NodeVisitor
{
public:
   getWorldCoordOfNodeVisitor():
      osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false)
      {
         wcMatrix= new osg::Matrixd();
      }
      virtual void apply(osg::Node &node)
      {
         if (!done)
         {
            if ( 0 == node.getNumParents() ) // no parents
            {
               wcMatrix->set( osg::computeLocalToWorld(this->getNodePath()) );
               done = true;
            }
            traverse(node);
         }
      }
      osg::Matrixd* giveUpDaMat()
      {
         return wcMatrix;
      }
private:
   bool done;
   osg::Matrix* wcMatrix;
};

// Given a valid node placed in a scene under a transform, return the
// world coordinates in an osg::Matrix.
// Creates a visitor that will update a matrix representing world coordinates
// of the node, return this matrix.
// (This could be a class member for something derived from node also.

osg::Matrixd* getWorldCoordinates( osg::Node* node)
{
   getWorldCoordOfNodeVisitor* ncv = new getWorldCoordOfNodeVisitor();
   if (node && ncv)
   {
      node->accept(*ncv);
      return ncv->giveUpDaMat();
   }
   else
   {
      return NULL;
   }
}




int main()
{
	root = new osg::Group();
	lightGroup = new osg::Group();
	root->addChild(lightGroup);

	//  Set up asset search paths
	osgDB::FilePathList pathList = osgDB::getDataFilePathList();
	pathList.push_back("/home/daniel/Documents/C++/ScumOfTheEarth/media/");
	osgDB::setDataFilePathList(pathList);

	windowWidth = 1600;
	windowHeight = 900;

	Level2D *level;
	level = new Level2D("media/smallTestMap(base64).tmx");
	addNewPlayer("thePlayer", osg::Vec3(0.0f, 0.0f, 0.0f));
	setActivePlayer("thePlayer");


	osg::Light* sun = new osg::Light;
	sun->setLightNum(0);
	sun->setPosition(osg::Vec4(0, 0, 200.0f, 1.0f));
	sun->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sun->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sun->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	osg::LightSource* lightSource = new osg::LightSource;
	lightSource->setLight(sun);
	lightGroup->addChild(lightSource);


	viewer.setSceneData(root);
	viewer.addEventHandler(getMainEventHandler());
	viewer.apply(new osgViewer::SingleWindow(0, 0, windowWidth, windowHeight, 0));
	viewer.setCameraManipulator(new TwoDimensionalCameraManipulator());
	//viewer.getCamera()->setProjectionMatrixAsOrtho2D(-10, 0, 0, 10 * windowHeight/windowWidth);
	viewer.getCamera()->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	viewer.getCamera()->setViewMatrix(osg::Matrix::identity());
	viewer.realize();


	getScriptEngine()->initialize();

	//getScriptEngine()->test();

	getScriptEngine()->runFile("initialize.as");




	AngelScriptConsole* console = new AngelScriptConsole();


	osg::Timer_t frame_tick = osg::Timer::instance()->tick();

	const int fpsArrayLength = 60;
	float fps[fpsArrayLength];
	for ( int i = 0; i < fpsArrayLength; ++i)
		fps[i] = 60;
	float fpsTotal = 60.0 * fpsArrayLength;

	while(!viewer.done())
	{
		osg::Timer_t now_tick = osg::Timer::instance()->tick();
		deltaTime = osg::Timer::instance()->delta_s(frame_tick, now_tick);
		frame_tick = now_tick;

		getCurrentLevel()->getPhysicsWorld()->Step(deltaTime, 6, 2);
		getCurrentLevel()->getDebugDrawer()->beginDraw();
		getCurrentLevel()->getPhysicsWorld()->DrawDebugData();
		getCurrentLevel()->getDebugDrawer()->endDraw();

		if (deltaTime > 0)
		{
			fpsTotal -= fps[0];
			for (int i = 0; i < fpsArrayLength - 1; ++i)
				fps[i] = fps[i+1];
			fps[fpsArrayLength - 1] = (1 / deltaTime);
			fpsTotal += fps[fpsArrayLength - 1];
		}

		std::ostringstream frameRateStream;
		frameRateStream << "FPS: " << (1 / deltaTime) << "(current)" << std::endl;
		frameRateStream << "FPS: " << (fpsTotal / fpsArrayLength) << "(average)"<< std::endl;
		getDebugDisplayer()->addText(frameRateStream);

		std::ostringstream playerCoordinatesStream;
		playerCoordinatesStream << "Player: " << getActivePlayer()->getPosition().x() << ", " << getActivePlayer()->getPosition().y() << ", " << getActivePlayer()->getPosition().z() << std::endl;
		getDebugDisplayer()->addText(playerCoordinatesStream);

		viewer.frame();

		removeExpiredObjects();

	}
	return 0;
}
