#include <iostream>

#include "globals.h"
#include "Level.h"

#ifdef TOP_DOWN_2D_VIEW
	#include "TwoDimensionalCameraManipulator.h"
#else
	#include "ThirdPersonCameraManipulator.h"
#endif
#include "AngelScriptEngine.h"
#include "AngelScriptConsole.h"
#include "TemporaryText.h"

#include "GameObjectData.h"


#include <osgViewer/config/SingleWindow>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>


using namespace std;

/// Variables declared extern in globals.h
osg::Group* root;
osg::Group* lightGroup;
int windowWidth, windowHeight;
double deltaTime;


Level* level;


osgAnimation::BasicAnimationManager* animationManager;

void logError(std::string errorMessage)
{
	std::cout << "Error: " << errorMessage << std::endl;
}
void logWarning(std::string warning)
{
	std::cout << "Warning: " << warning << std::endl;
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

float getDistance(osg::Vec3 a, osg::Vec3 b)
{
	return sqrt(pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2) + pow(a.z() - b.z(), 2));
}


osg::Vec3 cameraToWorldTranslation(float x, float y, float z)
{
	double camangle = ((BaseCameraManipulator*) getViewer()->getCameraManipulator())->getHeading();
	return osg::Vec3(x*cos(camangle) + y*cos(camangle+pi/2), x*sin(camangle) + y*sin(camangle+pi/2), z);
}
osg::Vec3 cameraToWorldTranslation(osg::Vec3 camTranslation)
{
	return cameraToWorldTranslation(camTranslation.x(), camTranslation.y(), camTranslation.z());
}


/// Visitor to return the world coordinates of a node.
/// It traverses from the starting node to the parent.
/// The first time it reaches a root node, it stores the world coordinates of the node it started from.  The world coordinates are found by concatenating all the matrix transforms found on the path from the start node to the root node.
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

/// Given a valid node placed in a scene under a transform, return the world coordinates in an osg::Matrix.
/// Creates a visitor that will update a matrix representing world coordinates of the node, return this matrix.
/// (This could be a class member for something derived from node also.)
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
      logWarning("Got NULL world coordinates");
   }
}


void GameOverYouLose()
{
	std::cout << "You Lose." << std::endl;
	new TemporaryText("You Lose", getActivePlayer()->getWorldPosition(), 5.0);
	// TODO: should actually end the game.
}

bool safeToAddRemoveNodes;
std::deque<std::pair<Node*,Group*>> nodesToAdd;

void addToSceneGraph(Node* node, Group* parent)
{
	if(safeToAddRemoveNodes)
		parent->addChild(node);
	else
	{
		nodesToAdd.push_back(std::pair<Node*,Group*>(node, parent));
	}
}
void addNodesToGraph()
{
	while(!nodesToAdd.empty())
	{
		std::pair<Node*,Group*> nodeAndParent = nodesToAdd.front();
		nodeAndParent.second->addChild(nodeAndParent.first);
		nodesToAdd.pop_front();
	}
}

void runCleanup()
{
	removeExpiredObjects();
}

void writeOutSceneGraph()
{
//	osgDB::writeNodeFile(*root, "output/sceneGraph.osg");
	level->saveAsYaml("output/exportedScene.yaml");

	GameObjectData::testYamlImportExport("output/exportedScene.yaml", "output/doublyExportedScene.yaml");
	level->reload("output/exportedScene.yaml");
}



osgViewer::Viewer* getViewer()
{
	static osgViewer::Viewer* viewer = new osgViewer::Viewer();
	return viewer;
}


int main()
{

	root = new osg::Group();
	lightGroup = new osg::Group();
	root->addChild(lightGroup);
	safeToAddRemoveNodes = true;

	//  Set up asset search paths
	osgDB::FilePathList pathList = osgDB::getDataFilePathList();
	pathList.push_back("/home/daniel/Documents/C++/ScumOfTheEarth/media/");
	pathList.push_back("/home/daniel/Documents/C++/ScumOfTheEarth/");
	osgDB::setDataFilePathList(pathList);

	windowWidth = 1600;
	windowHeight = 900;


	level = new Level("media/DemoLevel.yaml");


	osg::Light* sun = new osg::Light;
	sun->setLightNum(0);
	sun->setPosition(osg::Vec4(0, 0, 200.0f, 1.0f));
	sun->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sun->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sun->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	osg::LightSource* lightSource = new osg::LightSource;
	lightSource->setLight(sun);
	lightGroup->addChild(lightSource);


	getViewer()->setSceneData(root);
	getViewer()->addEventHandler(getMainEventHandler());
	getViewer()->apply(new osgViewer::SingleWindow(0, 0, windowWidth, windowHeight, 0));
#ifdef TOP_DOWN_2D_VIEW
	getViewer()->setCameraManipulator(new TwoDimensionalCameraManipulator());
	getViewer()->getCamera()->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	getViewer()->getCamera()->setViewMatrix(osg::Matrix::identity());
#else
	getViewer()->setCameraManipulator(new ThirdPersonCameraManipulator());
#endif

	osgViewer::StatsHandler* statsHandler = new osgViewer::StatsHandler();
	statsHandler->setKeyEventTogglesOnScreenStats( osgGA::GUIEventAdapter::KEY_F3);
	getViewer()->addEventHandler(statsHandler);
    getViewer()->addEventHandler(new osgViewer::WindowSizeHandler());
	getViewer()->realize();

	//getScriptEngine()->test();
	getScriptEngine()->runFile("initialize.as");

	AngelScriptConsole* console = new AngelScriptConsole();

	osg::Timer_t frame_tick = osg::Timer::instance()->tick();

	const int fpsArrayLength = 60;
	float fps[fpsArrayLength];
	for ( int i = 0; i < fpsArrayLength; ++i)
		fps[i] = 60;
	float fpsTotal = 60.0 * fpsArrayLength;

	addNodesToGraph();

	double elapsedTime = 0.0;	// Elapsed time, in seconds, that the game has been running for.

	while(!getViewer()->done())
	{
		osg::Timer_t now_tick = osg::Timer::instance()->tick();
		deltaTime = osg::Timer::instance()->delta_s(frame_tick, now_tick);
		frame_tick = now_tick;

		elapsedTime = osg::Timer::instance()->time_s();

		getCurrentLevel()->updatePhysics(deltaTime);
		if (deltaTime > 0)
		{
			fpsTotal -= fps[0];
			for (int i = 0; i < fpsArrayLength - 1; ++i)
				fps[i] = fps[i+1];
			fps[fpsArrayLength - 1] = (1 / deltaTime);
			fpsTotal += fps[fpsArrayLength - 1];
		}

		removeExpiredObjects();

		safeToAddRemoveNodes = false;
		getViewer()->frame(elapsedTime);
		safeToAddRemoveNodes = true;
		addNodesToGraph();
	}

	return 0;
}
