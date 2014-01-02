#include "TemporaryText.h"
#include "globals.h"

#include <osgText/Text>
#include "osg/PositionAttitudeTransform"

#include "OwnerUpdateCallback.h"

class TemporaryTextGeodeCallback : public osg::NodeCallback
{
public:
	TemporaryText* _owner;

	TemporaryTextGeodeCallback(TemporaryText* owner)
	{
		this->_owner = owner;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

TemporaryText::TemporaryText(std::string text, osg::Vec3 position, float duration)
{
	_transformNode = new osg::PositionAttitudeTransform();
	_transformNode->setPosition(position);
	_timeRemaining = duration;

	_text = new osgText::Text();
	_text->setText(text);
	_textGeode = new osg::Geode();

	_transformNode->addChild(_textGeode);
	addToSceneGraph(_transformNode);

	_textGeode->addDrawable(_text);
	osg::StateSet* stateSet = new osg::StateSet();
	_textGeode->setStateSet(stateSet);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateSet->setRenderBinDetails(11, "RenderBin");
	_text->setAxisAlignment(osgText::Text::SCREEN);
	_text->setCharacterSize(.25);
	_text->setFont("fonts/arial.ttf");
	_textGeode->setUpdateCallback(new OwnerUpdateCallback<TemporaryText>(this));


}

TemporaryText::~TemporaryText()
{
	_transformNode->getParent(0)->removeChild(_transformNode);	// remove the node from the scenegraph.
}

void TemporaryText::onUpdate(float deltaTime)
{
	this->_timeRemaining -= deltaTime;
	if(_timeRemaining <= 0)
		markForRemoval(this, "TemporaryText");
}
