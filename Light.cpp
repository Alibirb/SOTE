#include "Light.h"

#include "globals.h"

#include "tinyxml/tinyxml2.h"

#include "AngelScriptEngine.h"

#include "GameObjectData.h"

int Light::numLights = 0;

Light::Light(osg::Group* parentNode) : Attachment(parentNode)
{
	_objectType = "Light";

	//_lightNumber = numLights;
	_lightNumber = 1;
	numLights++;

	_lightGroup = new osg::Group();
	_light = new osg::Light;
	_light->setLightNum(_lightNumber);
	_light->setPosition(osg::Vec4(0, 0, 0, 1.0f));
	_light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 100.0f));
	_light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 100.0f));
	_light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 100.0f));
	//_light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	//_light->setSpotCutoff(1000);
	_lightSource = new osg::LightSource;
	_lightSource->setLight(_light);
	_lightGroup->addChild(_lightSource);

	glEnable(GL_LIGHT0 + _lightNumber);

	_transformNode->addChild(_lightGroup);


}
Light::Light(GameObjectData* dataObj, osg::Group* parentNode) : Light(parentNode)
{
	load(dataObj);
}

Light::~Light()
{
	//removeFromSceneGraph(_lightGroup);
	--numLights;
}


GameObjectData* Light::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);
	//saveAttachmentVariables(dataObj);
	saveLightVariables(dataObj);

	return dataObj;
}
void Light::saveLightVariables(GameObjectData* dataObj)
{
	dataObj->addData("ambient", _light->getAmbient());
	dataObj->addData("diffuse", _light->getDiffuse());
	dataObj->addData("specular", _light->getSpecular());
	dataObj->addData("constantAttenuation", _light->getConstantAttenuation());
	dataObj->addData("linearAttenuation", _light->getLinearAttenuation());
	dataObj->addData("quadraticAttenuation", _light->getQuadraticAttenuation());
}
void Light::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	//loadAttachmentVariables(dataObj);
	loadLightVariables(dataObj);
}

void Light::loadLightVariables(GameObjectData* dataObj)
{
	_light->setAmbient(dataObj->getVec4("ambient"));
	_light->setDiffuse(dataObj->getVec4("diffuse"));
	_light->setSpecular(dataObj->getVec4("specular"));
	_light->setConstantAttenuation(dataObj->getFloat("constantAttenuation"));
	_light->setLinearAttenuation(dataObj->getFloat("linearAttenuation"));
	_light->setQuadraticAttenuation(dataObj->getFloat("quadraticAttenuation"));
}


void Light::enableAllLights()
{
	for(int i=0; i < numLights; ++i)
		glEnable(GL_LIGHT0 + i);
}

