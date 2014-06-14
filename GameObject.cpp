#include "GameObject.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osg/Program>

#include "globals.h"
#include "Level.h"
#include "Sprite.h"
#include "OwnerUpdateCallback.h"
#include "AngelScriptEngine.h"
#include "GameObjectData.h"

#ifdef USE_BOX2D_PHYSICS
	#include "Box2D/Box2D.h"
	#include "Box2DIntegration.h"
	#include "Box2DDebugDrawer.h"
	#include "PhysicsContactListener.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
	#include "osgbCollision/GLDebugDrawer.h"
	#include "osgbCollision/CollisionShapes.h"
	#include "osgbDynamics/MotionState.h"
#endif
#include "PhysicsData.h"
#include "PhysicsNodeCallback.h"

#include "Attachment.h"

#include "ControlledObject.h"
#include "Door.h"
#include "Controller.h"
#include "Fighter.h"
#include "Player.h"
#include "Light.h"
#include "DangerZone.h"


struct AnimationManagerFinder : public osg::NodeVisitor
{
    osg::ref_ptr<ImprovedAnimationManager> _am;
    AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
    void apply(osg::Node& node) {
        if (_am.valid())
            return;
        if (node.getUpdateCallback()) {
            osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback());
            if (b) {
               _am = new ImprovedAnimationManager(*b);
                return;
            }
        }
        traverse(node);
    }
};


GameObject::GameObject(osg::Group* parentNode) : _physicsBody(NULL), _collisionShapeGenerationMethod("none"), _shaderProgram(NULL)
{
	registerGameObject();

	_objectType = "GameObject";

	//_transformNode = new osg::PositionAttitudeTransform();
	_transformNode = new ImprovedMatrixTransform();
	addToSceneGraph(_transformNode, parentNode);
	_updateNode = new osg::Node();
	_updateNode->addUpdateCallback(new OwnerUpdateCallback<GameObject>(this));	/// NOTE: Due to virtual inheritance, should be able to remove the template-ness of OwnerUpdateCallback.
	_transformNode->addChild(_updateNode);
}

GameObject::GameObject(GameObjectData* dataObj, osg::Group* parentNode) : GameObject(parentNode)
{
	load(dataObj);
}

GameObject::~GameObject()
{
	if(_transformNode->getNumParents() == 0)
		logError("GameObject with no parents found.");
	if(_transformNode->getNumParents() > 1)
		logError("GameObject with multiple parents found.");

	for(int i = 0; i < _transformNode->getNumParents(); ++i)
		_transformNode->getParent(i)->removeChild(_transformNode);	// remove the node from the scenegraph.

	if(_physicsBody)
	{
#ifdef USE_BOX2D_PHYSICS
		getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
#else
		getCurrentLevel()->getBulletWorld()->removeCollisionObject(_physicsBody);
		delete _physicsBody;
#endif
	}

	for(Attachment* attachment : _attachments)
		markForRemoval(attachment, attachment->getType());

	getCurrentLevel()->removeObject(this);
}

GameObject* GameObject::create(GameObjectData* dataObj, osg::Group* parentNode)
{
	#define GameObject_create_elseif(objectType) else if(type == #objectType) return new objectType (dataObj, parentNode);

	std::string type = dataObj->getType();
	if(type == "GameObject") return new GameObject(dataObj, parentNode);
	GameObject_create_elseif(ControlledObject)
	GameObject_create_elseif(Door)
	GameObject_create_elseif(Controller)
	GameObject_create_elseif(Fighter)
	GameObject_create_elseif(Player)

	GameObject_create_elseif(Light)
	GameObject_create_elseif(DangerZone)

	else
		logError("Unknown GameObject type '" + type + "' requested.");

	#undef GameObject_create_elseif
}




void GameObject::loadModel(std::string modelFilename)
{
	_modelFilename = modelFilename;
	if(modelFilename.length() > 4 && modelFilename.substr(modelFilename.length() - 4, 4) == ".png")
	{
		if(!osg::dynamic_pointer_cast<Sprite>(_modelNode))
			setModelNode(new Sprite(modelFilename));
		else
			osg::dynamic_pointer_cast<Sprite>(_modelNode)->setImage(modelFilename);
	}
	else
	{
        if(osgDB::readNodeFile(modelFilename))
            setModelNode(osgDB::readNodeFile(modelFilename));
		else
			logError("Could not load node file \"" + modelFilename + "\"");
	}
}
void GameObject::setModelNode(osg::Node* node)
{
	_transformNode->removeChild(_modelNode);
	_modelNode = node;
	_transformNode->addChild(_modelNode);
	findAnimation();

	tryToSetProperShaders(_modelNode);
}


void GameObject::setPosition(osg::Vec3 newPosition)
{
	this->_transformNode->setPosition(newPosition);
	if(_physicsBody)
	{
#ifdef USE_BOX2D_PHYSICS
		// TODO
#else
		osg::Vec3 localToWorldAdjustment = getWorldPosition() - getLocalPosition();
		btTransform transform;
		transform = _physicsBody->getWorldTransform();
		transform.setOrigin(osgbCollision::asBtVector3(localToWorldAdjustment + newPosition + physicsToModelAdjustment));	/// FIXME: should convert to world coordinates.
		_physicsBody->setWorldTransform(transform);
#endif // USE_BOX2D_PHYSICS
	}
}
void GameObject::setRotation(osg::Quat newRotation)
{
	this->_transformNode->setAttitude(newRotation);
	if(_physicsBody)
	{
#ifdef USE_BOX2D_PHYSICS
		// TODO
#else
		btTransform transform;
		transform = _physicsBody->getWorldTransform();
		//transform.setOrigin(osgbCollision::asBtVector3(newPosition + physicsToModelAdjustment));	/// FIXME: should convert to world coordinates.
		btVector4 vector4 = osgbCollision::asBtVector4(newRotation.asVec4());
		transform.setRotation(btQuaternion(vector4.x(), vector4.y(), vector4.z(), vector4.w()));
		_physicsBody->setWorldTransform(transform);
#endif // USE_BOX2D_PHYSICS
	}
}

osg::Vec3 GameObject::getLocalPosition()
{
	return _transformNode->getPosition();
}
osg::Vec3 GameObject::getWorldPosition()
{
	return getWorldCoordinates(_transformNode)->getTrans();
}
osg::Quat GameObject::getLocalRotation()
{
	return _transformNode->getAttitude();
}
osg::Quat GameObject::getWorldRotation()
{
	return getWorldCoordinates(_transformNode)->getRotate();
}
osg::Vec3 GameObject::localToWorld(osg::Vec3 localVector)	/// FIXME: does not seem to work.
{
	osg::NodePathList paths = _transformNode->getParentalNodePaths();
	osg::Matrix localToWorldMatrix = osg::computeLocalToWorld(paths.at(0));
	osg::Vec3f worldVector = localToWorldMatrix * localVector;
	return worldVector;
}
osg::Vec3 GameObject::worldToLocal(osg::Vec3 worldVector)	/// FIXME: May not work, either.
{
	osg::NodePathList paths = _transformNode->getParentalNodePaths();
	osg::Matrix worldToLocalMatrix = osg::computeWorldToLocal(paths.at(0));
	osg::Vec3f localVector = worldToLocalMatrix * worldVector;
	return localVector;
}


GameObjectData* GameObject::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);

	return dataObj;
}
void GameObject::saveGameObjectVariables(GameObjectData* dataObj)
{
	dataObj->addData("position", getLocalPosition());
	dataObj->addData("rotation", getLocalRotation());
	if(_modelFilename != "")
		dataObj->addData("geometry", _modelFilename);
	//dataObj->addData("autoGenerateCollisionBody", _autoGenerateCollisionBody);
	dataObj->addData("physicsBodyGeneration", _collisionShapeGenerationMethod);
	for(auto kv : _functionSources)
		dataObj->addScriptFunction(kv.first, kv.second);

	if(_physicsBody)
	{
#ifndef USE_BOX2D_PHYSICS
		if((btRigidBody*)_physicsBody)
		{
			if(((btRigidBody*)_physicsBody)->getInvMass() == 0)
				dataObj->addData("mass", 0.0);
			else
				dataObj->addData("mass", 1.0/((btRigidBody*)_physicsBody)->getInvMass());
			//dataObj->addData("mass", ((btRigidBody*)_physicsBody)->getInvMass());
		}
#endif
	}

	std::vector<Saveable*> attachmentVector;
	for(Attachment* attachment : _attachments)
		attachmentVector.push_back(attachment);
	dataObj->addData("attachments", attachmentVector);


	// TODO: animation.
}
void GameObject::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
}
void GameObject::loadGameObjectVariables(GameObjectData* dataObj)
{
	setPosition(dataObj->getVec3("position"));
	setRotation(dataObj->getQuat("rotation"));
	if(dataObj->hasString("geometry"))
		loadModel(dataObj->getString("geometry"));
	for(auto kv : dataObj->getFunctionSources())
		setFunction(kv.first, kv.second);
	_collisionShapeGenerationMethod = dataObj->getString("physicsBodyGeneration");
	if(!dataObj->hasString("physicsBodyGeneration") || _collisionShapeGenerationMethod == "")
		_collisionShapeGenerationMethod = "none";
	if(_collisionShapeGenerationMethod != "none")
		generateRigidBody(dataObj->getFloat("mass"), _collisionShapeGenerationMethod);

	for(auto attachmentData : dataObj->getObjectList("attachments"))
		addAttachment(Attachment::create(attachmentData, _transformNode));

}

void GameObject::addAttachment(Attachment* attachment)
{
	_attachments.push_back(attachment);
	attachment->parentTo(_transformNode);
}

void GameObject::reset()
{
	this->setPosition(initialPosition);
}

void GameObject::parentTo(osg::Group* parent)
{
	if(_transformNode->getNumParents() > 0)
	{
		logWarning("GameObject already parented.");
		_transformNode->getParent(0)->removeChild(_transformNode);	// Remove from current parent.
	}
	parent->addChild(_transformNode);
}
void GameObject::unparentFrom(osg::Group* parent)
{
	parent->removeChild(_transformNode);
}


bool GameObject::findAnimation()
{
	AnimationManagerFinder finder;
    _modelNode->accept(finder);
    if (finder._am.valid())
	{
        _modelNode->addUpdateCallback(finder._am.get());
        _animationManager = finder._am.get();
        std::cout << "Found animation." << std::endl;
        _animationManager->setPlayMode(osgAnimation::Animation::ONCE);
        return true;
    }
    else
	{
    	std::cout << "Did not find any animation." << std::endl;
        //osg::notify(osg::WARN) << "no osgAnimation::AnimationManagerBase found in the subgraph, no animations available" << std::endl;
        return false;
    }
}

void GameObject::playAnimation(std::string& animationName)
{
	if(!_animationManager)
	{
		logError("No animation manager found");
		return;
	}
	_animationManager->play(animationName);
	_animationManager->setPlayMode(osgAnimation::Animation::ONCE);
}


void GameObject::setFunction(std::string functionName, std::string code)
{
	_functionSources[functionName] = code;	/// Save the code so we can manipulate it if needed.
	_functions[functionName] = getScriptEngine()->compileFunction("GameObject", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}

void GameObject::generateRigidBody(double mass, std::string generationMethod)
{
#ifndef USE_BOX2D_PHYSICS
	btCollisionShape* shape;
	if(generationMethod == "triangleMeshShape")
		shape = osgbCollision::btTriMeshCollisionShapeFromOSG(_modelNode);
	else if(generationMethod == "convexHullShape")
		shape = osgbCollision::btConvexHullCollisionShapeFromOSG(_modelNode);
	else if(generationMethod == "boxShape")
		shape = osgbCollision::btBoxCollisionShapeFromOSG(_modelNode);
	else
		std::cout << "Could not determine collision shape generation method for \"" << generationMethod << "\"." << std::endl;

	btTransform transform = btTransform();
	transform.setIdentity();
	transform.setOrigin(osgbCollision::asBtVector3(getWorldPosition() + physicsToModelAdjustment));
	btVector4 vector4 = osgbCollision::asBtVector4(getWorldRotation().asVec4());
	transform.setRotation(btQuaternion(vector4.x(), vector4.y(), vector4.z(), vector4.w()));
	btVector3 localInertia;
	shape->calculateLocalInertia(mass, localInertia);

	osgbDynamics::MotionState* motionState = new osgbDynamics::MotionState();
	motionState->setTransform(_transformNode);
	motionState->setWorldTransform(transform);


	_physicsBody = new btRigidBody(mass, motionState, shape, localInertia);
	getCurrentLevel()->getBulletWorld()->addRigidBody((btRigidBody*)_physicsBody);

#endif

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = _objectType;
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	_physicsBody->setUserPointer(userData);
#endif
}

#ifndef USING_BOX2D_PHYSICS
btCollisionObject* GameObject::getPhysicsBody()
{
	return _physicsBody;
}
#endif

void GameObject::tryToSetProperShaders(osg::Node* node)
{
	if(node->asGeode())
	{
		/// Given node is a Geode. Look for drawables
		osg::Geode* geode = node->asGeode();
		for(int i = 0; i < geode->getNumDrawables(); ++i)
		{
			osg::Drawable* drawable = geode->getDrawable(i);
			if(drawable->getStateSet())
			{
				osg::StateSet* stateSet = drawable->getStateSet();
				if(stateSet->getNumTextureAttributeLists() > 1)
				{
					createShaders();
					stateSet->setAttribute( _shaderProgram, osg::StateAttribute::ON );
				}
			}
		}
	}

	if(node->asGroup())
	{
		osg::Group* group = node->asGroup();
		for(int i = 0; i < group->getNumChildren(); ++i)
			tryToSetProperShaders(group->getChild(i));
	}

}

void GameObject::createShaders()
{
	if(_shaderProgram && (_shaderProgram->getNumShaders() > 0) )
		return;	/// Already set up.

	/// Adapted from osgwnormalmap.cpp

	///Setup the shaders and programs
	std::string shaderName = osgDB::findDataFile( "parallax_mapping.fs" );
	osg::ref_ptr< osg::Shader > fragmentShader;
	if( shaderName.empty() )
	{
		std::string fragmentSource =
		"uniform sampler2D baseMap;\n"
		"uniform sampler2D normalMap;\n"
		"uniform sampler2D heightMap;\n"
		"uniform bool useHeightMap;\n"
		"\n"
		"varying vec3 v_lightVector;\n"
		"varying vec3 v_viewVector;\n"
		"\n"
		"void main()\n"
		"{\n"
		//determine if we are going to use the height map
		"float height = 0.;\n"
		"float v = 0.;\n"
		"if( useHeightMap )\n"
		"{\n"
			"height = texture2D( heightMap, gl_TexCoord[ 0 ].st ).r;\n"
			"vec2 scaleBias = vec2( 0.06, 0.03 );\n"
			"v = height * scaleBias.s - scaleBias.t;\n"
		"}\n"
		"\n"
		"vec3 V = normalize( v_viewVector );\n"
		"vec2 texCoords = gl_TexCoord[ 0 ].st + ( V.xy * v );\n"
		"\n"
		//
		"float bumpiness = 1.0;\n"
		"vec3 smoothOut = vec3( 0.5, 0.5, 1.0 );\n"
		"vec3 N = texture2D( normalMap, texCoords ).rgb;\n"
		"N = mix( smoothOut, N, bumpiness );\n"
		"N = normalize( ( N * 2.0 ) - 1.0 );\n"
		"\n"
		//
		"vec3 L = normalize( v_lightVector );\n"
		"float NdotL = max( dot( N, L ), 0.0 );\n"
		"\n"
		//
		"vec3 R = reflect( V, N );\n"
		"float RdotL = max( dot( R, L ), 0.0 );\n"
		"\n"
		//
		"float specularPower = 50.0;\n"
		"vec3 base = texture2D( baseMap, texCoords ).rgb;\n"
		"vec3 ambient = vec3( 0.368627, 0.368421, 0.368421 ) * base;\n"
		"vec3 diffuse = vec3( 0.886275, 0.885003, 0.885003 ) * base * NdotL;\n"
		"vec3 specular = vec3( 0.490196, 0.488722, 0.488722 ) * pow( RdotL, specularPower );\n"
		"vec3 color = ambient + diffuse + specular;\n"
		"\n"
		//
		"gl_FragColor = vec4( color, 1.0 );\n"
		"}\n";
		fragmentShader = new osg::Shader();
		fragmentShader->setType( osg::Shader::FRAGMENT );
		fragmentShader->setShaderSource( fragmentSource );
		osg::notify( osg::ALWAYS ) << "Using the inline fragment shader." << std::endl;
	}
	else
	{
		fragmentShader =
			osg::Shader::readShaderFile( osg::Shader::FRAGMENT, shaderName );
		osg::notify( osg::ALWAYS ) << "Using the file fragment shader." << std::endl;
	}
	fragmentShader->setName( "parallax frag shader" );

	shaderName = osgDB::findDataFile( "parallax_mapping.vs" );
	osg::ref_ptr< osg::Shader > vertexShader;
	if( shaderName.empty() )
	{
		std::string vertexSource =
		"attribute vec4 a_tangent; \n"
		"attribute vec4 a_binormal;\n"

		"varying vec3 v_lightVector;\n"
		"varying vec3 v_viewVector;\n"
		"\n"
		"void main()\n"
		"{\n"
		//
		"gl_Position = ftransform();\n"
		"\n"
		//Get the texture coordinates
		"gl_TexCoord[ 0 ] = gl_TextureMatrix[ 0 ] * gl_MultiTexCoord0;\n"
		"\n"
		//Convert the vertex position into eye coordinates
		"vec3 ecPosition = vec3( gl_ModelViewMatrix * gl_Vertex );\n"
		"\n"
		//Convert tangent, binormal, and normal into eye coordinates
		"mat3 TBNMatrix = mat3( gl_ModelViewMatrix[0].xyz,gl_ModelViewMatrix[1].xyz,gl_ModelViewMatrix[2].xyz ) *\n"
		"    mat3( a_tangent.xyz, a_binormal.xyz, gl_Normal );\n"
		"\n"
		//Convert light vector into tangent space
		"v_lightVector = gl_LightSource[ 0 ].position.xyz - ecPosition;\n"
		"v_lightVector *= TBNMatrix;\n"
		"\n"
		//Convert view vector into tangent space
		"v_viewVector = ecPosition;\n"
		"v_viewVector *= TBNMatrix;\n"
		"}\n";
		vertexShader = new osg::Shader();
		vertexShader->setType( osg::Shader::VERTEX );
		vertexShader->setShaderSource( vertexSource );
		osg::notify( osg::ALWAYS ) << "Using the inline vertex shader." << std::endl;
	}
	else
	{
		vertexShader = osg::Shader::readShaderFile( osg::Shader::VERTEX, shaderName );
		osg::notify( osg::ALWAYS ) << "Using the file vertex shader." << std::endl;
	}
	vertexShader->setName( "parallax vertex shader" );

	_shaderProgram = new osg::Program();
	_shaderProgram->addShader( vertexShader.get() );
	_shaderProgram->addShader( fragmentShader.get() );
	_shaderProgram->addBindAttribLocation( "a_tangent", 6 );
	_shaderProgram->addBindAttribLocation( "a_binormal", 7 );
	_shaderProgram->addBindAttribLocation( "a_normal", 15 );
}



namespace AngelScriptWrapperFunctions
{
	GameObject* GameObjectFactoryFunction()
	{
		GameObjectData* dataObj = new GameObjectData("GameObject");
		return GameObject::create(dataObj, root);
		//return new GameObject(root);
	}
}

using namespace AngelScriptWrapperFunctions;


void registerGameObject()
{
	static bool registered = false;
	if(registered)
		return;

	getScriptEngine()->registerObjectType("GameObject", sizeof(GameObject), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("GameObject", "GameObject@ f()", asFUNCTION(GameObjectFactoryFunction));

	getScriptEngine()->registerObjectMethod("GameObject", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	registered = true;
}


