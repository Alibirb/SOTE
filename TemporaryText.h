#ifndef TEMPORARYTEXT_H
#define TEMPORARYTEXT_H

#include <iostream>
#include <list>

namespace osgText {
	class Text;
}
namespace osg {
	class Vec3f;
	typedef Vec3f Vec3;
	class Geode;
	class PositionAttitudeTransform;
}

/// Class for text that will disappear after a specified time
class TemporaryText
{
private:
	osgText::Text* _text;
	osg::Geode* _textGeode;
	osg::PositionAttitudeTransform* _transformNode;
	float _timeRemaining;

public:
	TemporaryText(std::string text, osg::Vec3 position, float duration);
	virtual ~TemporaryText();
	void onUpdate(float deltaTime);

protected:
private:
};

std::list<TemporaryText*> getTempTexts();

#endif // TEMPORARYTEXT_H
