#ifndef LIGHT_H
#define LIGHT_H

//#include "ControlledObject.h"
#include "Attachment.h"

#include <osg/LightSource>


class Light : public Attachment
{
private:
	osg::ref_ptr<osg::Light> _light;
	osg::ref_ptr<osg::LightSource> _lightSource;
	osg::ref_ptr<osg::Group> _lightGroup;

	int _lightNumber;	/// OpenGL light number of this light.
	static int numLights;	/// Number of currently-existing lights. Used to ensure each Light gets a unique number.

public:
	Light(osg::Group* parentNode);
	Light(GameObjectData* dataObj, osg::Group* parentNode);
	virtual ~Light();

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

	static void enableAllLights();

protected:
	void saveLightVariables(GameObjectData* dataObj);
	void loadLightVariables(GameObjectData* dataObj);
};

#endif // LIGHT_H
