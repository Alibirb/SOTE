#ifndef LIGHT_H
#define LIGHT_H

#include "ControlledObject.h"

#include <osg/LightSource>


class Light : public ControlledObject
{
private:
	osg::Light* _light;
	osg::LightSource* _lightSource;
	osg::Group* _lightGroup;

	int _lightNumber;	/// OpenGL light number of this light.
	static int numLights;	/// Number of currently-existing lights. Used to ensure each Light gets a unique number.

public:
	Light();
	Light(GameObjectData* dataObj);
	virtual ~Light();

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

	static void enableAllLights();

protected:
	void saveLightVariables(GameObjectData* dataObj);
	void loadLightVariables(GameObjectData* dataObj);
};

#endif // LIGHT_H
