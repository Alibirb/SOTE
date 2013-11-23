/*
 * TextDisplay.h
 *
 *  Created on: Aug 21, 2013
 *      Author: daniel
 */

#ifndef TEXTDISPLAY_H_
#define TEXTDISPLAY_H_

#include <osgText/Text>
#include <iostream>
#include <sstream>

class TextDisplay
{
public:
	std::ostringstream stream;
	osgText::Text* text;
	osg::Projection* projectionMatrix;
	osg::MatrixTransform* viewMatrix;
	osg::Geode* textGeode;

	TextDisplay();

	void updateProjection();

	void setDefaultText();

	void addText(std::string newText)
	{
		stream << newText;
	}
	void addText(std::ostringstream& newText)
	{
		stream << newText.str();
	}
	void addText(int newText)
	{
		stream << newText;
	}
	void addText(float newText)
	{
		stream << newText;
	}
};

class TextGeodeCallback : public osg::NodeCallback
{
public:
	TextDisplay* display;

	TextGeodeCallback(TextDisplay* display)
	{
		this->display = display;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		//display->setDefaultText();
		display->text->setText(display->stream.str());
	//	display->stream.str(std::string() );	// set to a blank string
	//	display->stream.clear();
		display->setDefaultText();

		traverse(node, nv);	// need to call this so scene graph traversal continues.
	}
};

TextDisplay* getDebugDisplayer();


#endif /* TEXTDISPLAY_H_ */
