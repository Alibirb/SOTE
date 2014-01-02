/*
 * TextDisplay.h
 *
 *  Created on: Aug 21, 2013
 *      Author: daniel
 */

#ifndef TEXTDISPLAY_H_
#define TEXTDISPLAY_H_


#include <iostream>
#include <sstream>

namespace osgText {
	class Text;
}

namespace osg {
	class Projection;
	class MatrixTransform;
	class Geode;
	class Vec3f;
	typedef Vec3f Vec3;
}

class TextDisplay	/// TODO: Change name to HudText?
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

	void addText(std::string newText);
	void addText(std::ostringstream& newText);
	void addText(int newText);
	void addText(float newText);
	void addText(osg::Vec3 coordinates);
};



TextDisplay* getDebugDisplayer();


#endif /* TEXTDISPLAY_H_ */
