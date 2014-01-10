/*
 * TextDisplay.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: daniel
 */

#include "TextDisplay.h"

#include <osgText/Text>
#include <osg/Projection>
#include <osg/MatrixTransform>


#include "globals.h"


class TextGeodeCallback : public osg::NodeCallback
{
public:
	TextDisplay* _display;

	TextGeodeCallback(TextDisplay* display)
	{
		this->_display = display;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

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
	//root->addChild(projectionMatrix);
	addToSceneGraph(projectionMatrix);
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

TextDisplay::~TextDisplay()
{
	projectionMatrix->getParent(0)->removeChild(projectionMatrix);	// remove the node from the scenegraph.
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
}

void TextDisplay::addText(std::string newText)
{
	stream << newText;
}
void TextDisplay::addText(std::ostringstream& newText)
{
	stream << newText.str();
}
void TextDisplay::addText(int newText)
{
	stream << newText;
}
void TextDisplay::addText(float newText)
{
	stream << newText;
}
void TextDisplay::addText(osg::Vec3 coordinates)
{
	stream << coordinates.x() << ", " << coordinates.y() << ", " << coordinates.z();
}






void TextGeodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	_display->text->setText(_display->stream.str());
	_display->setDefaultText();

	traverse(node, nv);	// need to call this so scene graph traversal continues.
}


TextDisplay* getDebugDisplayer()
{
	static TextDisplay* debugDisplayer = new TextDisplay();
	return debugDisplayer;
}
