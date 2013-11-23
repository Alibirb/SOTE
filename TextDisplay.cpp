/*
 * TextDisplay.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: daniel
 */

#include "TextDisplay.h"
#include "osg/Projection"
#include "globals.h"

TextDisplay::TextDisplay()
{
	text = new osgText::Text();
	textGeode = new osg::Geode();
	projectionMatrix = new osg::Projection;
	int x, y, width, height;
	((osgViewer::GraphicsWindow* )viewer.getCamera()->getGraphicsContext())->getWindowRectangle(x, y, width, height);
	projectionMatrix->setMatrix(osg::Matrix::ortho2D(x, windowWidth, y, windowHeight));
	viewMatrix = new osg::MatrixTransform;
	viewMatrix->setMatrix(osg::Matrix::identity());
	viewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	root->addChild(projectionMatrix);
	projectionMatrix->addChild(viewMatrix);
	viewMatrix->addChild(textGeode);
	textGeode->addDrawable(text);
	osg::StateSet* stateSet = new osg::StateSet();
	textGeode->setStateSet(stateSet);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateSet->setRenderBinDetails(11, "RenderBin");
	text->setAxisAlignment(osgText::Text::SCREEN);
	text->setPosition(osg::Vec3(0, height - 25, 0));
	text->setCharacterSize(25);
	text->setFont("fonts/arial.ttf");
	textGeode->setUpdateCallback(new TextGeodeCallback(this));
	setDefaultText();
}

void TextDisplay::updateProjection()
{
	int x, y, width, height;
	((osgViewer::GraphicsWindow* )viewer.getCamera()->getGraphicsContext())->getWindowRectangle(x, y, width, height);
	projectionMatrix->setMatrix(osg::Matrix::ortho2D(x, windowWidth, y, windowHeight));
	text->setPosition(osg::Vec3(0, height - 25, 0));
}

void TextDisplay::setDefaultText()
{
	stream.str(std::string() );	// set to a blank string
	stream.clear();	// clears error state flag

	//NOTE: camManipulator is actually global right now. Should figure out what exactly I want to do with it.
	//stream << "camAzimuth: " << ((ThirdPersonCameraManipulator*)viewer.getCameraManipulator())->getHeading() << std::endl;
	//stream << "camZenith: " << camManipulator->getZenith() << std::endl;
	//stream << "Player Pos: " << getActivePlayer()->getPosition().x() << "," << getActivePlayer()->getPosition().y() << "," << getActivePlayer()->getPosition().z() << std::endl;

}


TextDisplay* getDebugDisplayer()
{
	static TextDisplay* debugDisplayer = new TextDisplay();
	return debugDisplayer;
}
