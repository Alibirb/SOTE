#include "AngelScriptEngine.h"
#include "add_on/scriptstdstring/scriptstdstring.h"
#include "add_on/scriptarray/scriptarray.h"

#include "stdvector.h"

#include <osg/Vec3>

#include "Enemy.h"
#include "Player.h"
#include "Weapon.h"

using namespace osg;

namespace AngelScriptWrapperFunctions
{
	void Vec3Constructor(Vec3 *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) Vec3();
	}
	void Vec3InitConstructor(float x, float y, float z, Vec3 *self)
	{
		new(self) Vec3(x, y, z);
	}
	void Vec3CopyConstructor(Vec3& other, Vec3* self)
	{
		new(self) Vec3(other.x(), other.y(), other.z());
	}
	void Vec3Destructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((osg::Vec3*)memory)->~Vec3();
	}
}

using namespace AngelScriptWrapperFunctions;


DWORD timeGetTime()
{
	timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec*1000 + time.tv_usec/1000;
}

void print(std::string& msg)
{
	std::cout << msg << std::endl;
}
void printFloat(float value)
{
	std::cout << value << std::endl;
}


void createEnemy(std::string& name, osg::Vec3& position)
{
	new Enemy(name, position);
}


void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING )
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";

	printf("%s (row %d, col %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}


AngelScriptEngine::AngelScriptEngine()
{
	initialize();
}

AngelScriptEngine::~AngelScriptEngine()
{
	close();
}

void AngelScriptEngine::registerDefaultStuff()
{
	// Register the standard string type.
	// There's no definitive standard string type for C++, so AngelScript doesn't have a built-in string type. The application must define its own.
	RegisterStdString(engine);

	RegisterScriptArray(engine, true);

	int r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
	assert(r >= 0);
	r = engine->RegisterGlobalFunction("void print(float)", asFUNCTION(printFloat), asCALL_CDECL);
	assert(r >= 0);

	registerObjectType("Vec3", sizeof(osg::Vec3), asOBJ_VALUE | GetTypeTraits<osg::Vec3>() | asOBJ_APP_CLASS_ALLFLOATS, asFUNCTION(Vec3Constructor), asFUNCTION(Vec3Destructor));
	registerConstructor("Vec3", "void f(float, float, float)", asFUNCTION(Vec3InitConstructor));
	registerConstructor("Vec3", "void f(const Vec3 &in)", asFUNCTION(Vec3CopyConstructor));
	registerObjectProperty("Vec3", "float x", asOFFSET(Vec3, _v[0]));
	registerObjectProperty("Vec3", "float y", asOFFSET(Vec3, _v[1]));
	registerObjectProperty("Vec3", "float z", asOFFSET(Vec3, _v[2]));
	registerObjectMethod("Vec3", "Vec3 opAdd(const Vec3 &in) const", asMETHODPR(Vec3, operator+, (const Vec3 &) const, const Vec3), asCALL_THISCALL);
	registerObjectMethod("Vec3", "Vec3 opSub(const Vec3 &in) const", asMETHODPR(Vec3, operator-, (const Vec3 &) const, const Vec3), asCALL_THISCALL);

	registerFunction("void createEnemy(string &in, Vec3 &in)", asFUNCTION(createEnemy));
}

void setWeaponAngle(float angle)
{
	getActivePlayer()->getWeapon()->setRotation(angle);
}

void AngelScriptEngine::registerTestingStuff()
{
	registerFunction("void setWeaponAngle(float)", asFUNCTION(setWeaponAngle));
}

/// Function to run tests of the scripting system.
void AngelScriptEngine::test()
{
	runFile("tests.as");

	runFunction("void run()");

	runFunction("float calc(float a, float b)", "float", 1.25, "float", 2.125);
	std::cout << getReturnFloat() << std::endl;
	osg::Vec3 a = osg::Vec3(0,5, 3.5);
	osg::Vec3 b = osg::Vec3(-9, 3, 2.4);

	runFunction("Vec3 calc(Vec3 &in a, Vec3 &in b)", "object", &a, "object", &b);
	osg::Vec3 rtn = *(osg::Vec3*)getReturnObject();
	std::cout << rtn.x() << " " << rtn.y() << " " << rtn.z() << std::endl;

	runFunction("void calcAndPrint(Vec3 &in a, Vec3 &in b)", "object", &a, "object", &b);

	runFunction("void print(Vec3 &in a)", "object", &a);

}


bool AngelScriptEngine::initialize()
{
	// Create the script engine.
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	// Set the message callback to receive information on errors in human readable form.
	int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	assert(r >= 0);

	r = builder.StartNewModule(engine, "MyModule");
	if (r < 0 )
	{
		// If the code fails here it is usually because there is more memory to allocate the module.
		std::cout << "Unrecoverable error while starting a new module." << std::endl;
		return false;
	}
	r = builder.BuildModule();
	if (r < 0)
	{
		// An error occured. Instruct the script writer to fix the compilation errors that listed in the output stream.
		std::cout << "Please correct the errors in the script and try again." << std::endl;
	}

	ctx = engine->CreateContext();

	// Find the function that is to be called.
	mod = engine->GetModule("MyModule");

	registerDefaultStuff();
	registerTestingStuff();
	//runTests();

	return true;
}

bool AngelScriptEngine::close()
{
	// Clean up
	ctx->Release();
	engine->Release();
	return true;
}


bool AngelScriptEngine::eval(const std::string& code)
{
	//int r = ExecuteString(engine, code.c_str(), mod, ctx);
	int r = ExecuteString(engine, code.c_str(), mod);
	if(r != asEXECUTION_FINISHED)
	{
		if(r == asEXECUTION_EXCEPTION)
		{
			string warning = "AngelScript engine says \"";
			warning += ctx->GetExceptionString();
			warning += "\".";
			logWarning(warning);
			return false;
		}
	}

	return true;
}


bool AngelScriptEngine::runFile(const std::string& filePath, const char* functionName)
{
	int r = builder.StartNewModule(engine, "MyModule");
	if (r < 0 )
	{
		// If the code fails here it is usually because there is more memory to allocate the module.
		std::cout << "Unrecoverable error while starting a new module." << std::endl;
		return false;
	}
	//int
	r = builder.AddSectionFromFile(filePath.c_str());
	if (r < 0)
	{
		// The builder wasn't able to load the file. Maybe the file has been removed, or the wrong name was given, or some preprocessing commands are incorrectly written.
		std::cout << "Please correct the errors in the script and try again." << std::endl;
		return false;
	}
	r = builder.BuildModule();
	if (r < 0)
	{
		// An error occured. Instruct the script writer to fix the compilation errors that listed in the output stream.
		std::cout << "Please correct the errors in the script and try again." << std::endl;
	}
/*
	// Find the function that is to be called.
	//mod = engine->GetModule("MyModule");
	//mod = builder.GetModule();
	asIScriptFunction *func = mod->GetFunctionByDecl(functionName);
	if (func == 0)
	{
		// The function couldn't be found. Instruct the script writer to include the expected function in the script.
		std::cout << "The script \"" << filePath << "\" must have the function \"" << functionName << "\"." << std::endl;
		return false;
	}

	// Create our context, prepare it, and then execute.
	ctx = engine->CreateContext();
	ctx->Prepare(func);
	r = ctx->Execute();
	if (r != asEXECUTION_FINISHED)
	{
		// The execution didn't complete as expected. Determine what happened.
		if (r == asEXECUTION_EXCEPTION)
		{
			// An exception occured, let the script writer know what happened so it can be corrected.
			string warning = "AngelScript engine says \"";
			warning += ctx->GetExceptionString();
			warning += "\".";
			logWarning(warning);
			return false;
		}
	}
	func->Release();
*/
	runFunction(functionName);
	return true;
}
/*
template<typename T>
void AngelScriptEngine::registerVector(std::string vectorName, std::string typeName)
{
	RegisterVector<T>(vectorName, typeName, engine);
}
*/

void AngelScriptEngine::registerTypedef(const char * type, const char * decl)
{
	int r = engine->RegisterTypedef(type, decl);
	assert(r >= 0);
}



AngelScriptEngine* getScriptEngine()
{
	static AngelScriptEngine* engine = new AngelScriptEngine();
	return engine;
}
