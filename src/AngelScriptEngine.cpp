#include "AngelScriptEngine.h"
#include "scriptstdstring/scriptstdstring.h"
#include "scriptarray/scriptarray.h"

#include "stdvector.h"

#include <osg/Vec3>

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


void print(std::string& msg)
{
	std::cout << msg << std::endl;
}
void printFloat(float value)
{
	std::cout << value << std::endl;
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
	engine->Release();
	return true;
}


asIScriptContext* AngelScriptEngine::getContextFromPool()
{
	// Get a context from the pool, or create a new one
	asIScriptContext *ctx = 0;
	if( pool.size() )
	{
		ctx = *pool.rbegin();
		pool.pop_back();
	}
	else
		ctx = engine->CreateContext();
	return ctx;
}

void AngelScriptEngine::returnContextToPool(asIScriptContext *ctx)
{
	pool.push_back(ctx);
	// Unprepare the context to free non-reusable resources
	//ctx->Unprepare();
}

void AngelScriptEngine::registerVec3()
{
	registerObjectType("Vec3", sizeof(osg::Vec3), asOBJ_VALUE | GetTypeTraits<osg::Vec3>() | asOBJ_APP_CLASS_ALLFLOATS, asFUNCTION(Vec3Constructor), asFUNCTION(Vec3Destructor));
	registerConstructor("Vec3", "void f(float, float, float)", asFUNCTION(Vec3InitConstructor));
	registerConstructor("Vec3", "void f(const Vec3 &in)", asFUNCTION(Vec3CopyConstructor));
	registerObjectProperty("Vec3", "float x", asOFFSET(Vec3, _v[0]));
	registerObjectProperty("Vec3", "float y", asOFFSET(Vec3, _v[1]));
	registerObjectProperty("Vec3", "float z", asOFFSET(Vec3, _v[2]));
	registerObjectMethod("Vec3", "Vec3 opAdd(const Vec3 &in) const", asMETHODPR(Vec3, operator+, (const Vec3 &) const, const Vec3), asCALL_THISCALL);
	registerObjectMethod("Vec3", "Vec3 opSub(const Vec3 &in) const", asMETHODPR(Vec3, operator-, (const Vec3 &) const, const Vec3), asCALL_THISCALL);
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

	registerVec3();

}


void AngelScriptEngine::registerTestingStuff()
{

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





bool AngelScriptEngine::eval(const std::string& code)
{
	asIScriptContext* ctx;
	ctx = getContextFromPool();

	int r = ExecuteString(engine, code.c_str(), mod, ctx);


	_returnObject = ctx->GetReturnObject();	/// Save the return object (which may not exist)
	_returnValue = ctx->GetReturnQWord();

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
	returnContextToPool(ctx);

	return true;
}



bool AngelScriptEngine::runFile(const std::string& filePath, const char* functionName)
{
	if(!engine->GetModule(filePath.c_str()))	/// Only build the module if it doesn't already exist.
	{
		int r = builder.StartNewModule(engine, filePath.c_str());
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
	}
	mod = engine->GetModule(filePath.c_str());

	runFunction(functionName);
	return true;
}



void AngelScriptEngine::addFunction(const char* sectionName, const char* code, int lineOffset, asDWORD compileFlags, asIScriptFunction** outFunc)
{
	mod->CompileFunction(sectionName, code, lineOffset, compileFlags, outFunc);
}

asIScriptFunction* AngelScriptEngine::compileFunction(const char* sectionName, const char* code, int lineOffset, asDWORD compileFlags)
{
	asIScriptFunction* func;
	mod->CompileFunction(sectionName, code, lineOffset, compileFlags, &func);
	return func;
}

void AngelScriptEngine::registerFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	int r = engine->RegisterGlobalFunction(declaration, funcPointer, callConv);
	if(r < 0)
		logError("Could not register function.");
}

/// Register a global variable. Note that you must pass in a pointer to whatever you want. Even if it's already a pointer.
void AngelScriptEngine::registerProperty(const char *declaration, void *pointer)
{
	int r = engine->RegisterGlobalProperty(declaration, pointer);
	if(r < 0)
		logError("Could not register property.");
}

/// Registers an object type.
/// See http://www.angelcode.com/angelscript/sdk/docs/manual/doc_register_val_type.html for explanation of how it works.
/// Use GetTypeTraits<type>() to automatically determine the appropriate traits. (With C++11 compiler).
void AngelScriptEngine::registerObjectType(const char *obj, int byteSize, asDWORD flags)
{
	int r = engine->RegisterObjectType(obj, byteSize, flags);
	if(r < 0)
		logError("Could not register object type.");
	if(r < 0)
		switch(r)
		{
		case asINVALID_ARG:
			cout << "Invalid argument" << " (the flags are invalid)" << std::endl;
			break;
		case asINVALID_NAME:
			cout << "Invalid name" << std::endl;
			break;
		case asALREADY_REGISTERED:
			cout << "Already registered" << std::endl;
			break;
		case asNAME_TAKEN:
			cout << "Name taken" << std::endl;
			break;
		case asLOWER_ARRAY_DIMENSION_NOT_REGISTERED:
			cout << "Lower array dimension not registered" << std::endl;
			break;
		case asINVALID_TYPE:
			cout << "Invalid type" << std::endl;
			break;
		case asNOT_SUPPORTED:
			cout << "Not supported" << std::endl;
			break;
		}
}


/// Registers an object type, along with default constructor and destructor.
/// See http://www.angelcode.com/angelscript/sdk/docs/manual/doc_register_val_type.html for explanation of how it works.
/// Use GetTypeTraits<type>() to automatically determine the appropriate traits. (With C++11 compiler).
void AngelScriptEngine::registerObjectType(const char *obj, int byteSize, asDWORD flags, const asSFuncPtr &constructorPointer, const asSFuncPtr &destructorPointer)
{
	registerObjectType(obj, byteSize, flags);

	registerConstructor(obj, "void f()", constructorPointer);
	registerDestructor(obj, destructorPointer);
}


/// Registers a factory function. For use by reference types.
void AngelScriptEngine::registerFactoryFunction(const char *obj, const char *declaration, const asSFuncPtr &funcPointer)
{
	int r = engine->RegisterObjectBehaviour(obj, asBEHAVE_FACTORY, declaration, funcPointer, asCALL_CDECL);
	if(r < 0)
		switch(r)
		{
		case asWRONG_CONFIG_GROUP:
			cout << "Wrong config group" << std::endl;
			break;
		case asNOT_SUPPORTED:
			cout << "Not supported" << std::endl;
			break;
		case asINVALID_TYPE:
			cout << "Invalid type" << std::endl;
			break;
		case asINVALID_DECLARATION:
			cout << "Invalid declaration" << std::endl;
			break;
		case asNAME_TAKEN:
			cout << "Name taken" << std::endl;
			break;
		case asWRONG_CALLING_CONV:
			cout << "Wrong calling convention" << std::endl;
			break;
		case asALREADY_REGISTERED:
			cout << "Already registered" << std::endl;
			break;
		case asILLEGAL_BEHAVIOUR_FOR_TYPE:
			cout << "Illegal behaviour for type" << std::endl;
			break;
		case asINVALID_ARG:
			cout << "Invalid argument" << std::endl;
			break;

		}
}

void AngelScriptEngine::registerDestructor(const char *obj, const asSFuncPtr &funcPointer)
{
	int r = engine->RegisterObjectBehaviour(obj, asBEHAVE_DESTRUCT, "void f()", funcPointer, asCALL_CDECL_OBJLAST);
	if(r < 0)
		switch(r)
		{
		case asWRONG_CONFIG_GROUP:
			cout << "Wrong config group" << std::endl;
			break;
		case asNOT_SUPPORTED:
			cout << "Not supported" << std::endl;
			break;
		case asINVALID_TYPE:
			cout << "Invalid type" << std::endl;
			break;
		case asINVALID_DECLARATION:
			cout << "Invalid declaration" << std::endl;
			break;
		case asNAME_TAKEN:
			cout << "Name taken" << std::endl;
			break;
		case asWRONG_CALLING_CONV:
			cout << "Wrong calling convention" << std::endl;
			break;
		case asALREADY_REGISTERED:
			cout << "Already registered" << std::endl;
			break;
		case asILLEGAL_BEHAVIOUR_FOR_TYPE:
			cout << "Illegal behaviour for type." << std::endl;
			break;
		case asINVALID_ARG:
			cout << "Invalid argument" << std::endl;
			break;

		}
}

/// Registers a constructor. For use by value types.
void AngelScriptEngine::registerConstructor(const char *obj, const char *declaration, const asSFuncPtr &funcPointer)
{
	int r = engine->RegisterObjectBehaviour(obj, asBEHAVE_CONSTRUCT, declaration, funcPointer, asCALL_CDECL_OBJLAST);
	if(r < 0)
		switch(r)
		{
		case asWRONG_CONFIG_GROUP:
			cout << "Wrong config group" << std::endl;
			break;
		case asNOT_SUPPORTED:
			cout << "Not supported" << std::endl;
			break;
		case asINVALID_TYPE:
			cout << "Invalid type" << std::endl;
			break;
		case asINVALID_DECLARATION:
			cout << "Invalid declaration" << std::endl;
			break;
		case asNAME_TAKEN:
			cout << "Name taken" << std::endl;
			break;
		case asWRONG_CALLING_CONV:
			cout << "Wrong calling convention" << std::endl;
			break;
		case asALREADY_REGISTERED:
			cout << "Already registered" << std::endl;
			break;
		case asILLEGAL_BEHAVIOUR_FOR_TYPE:
			cout << "Illegal behaviour for type" << std::endl;
			break;
		case asINVALID_ARG:
			cout << "Invalid argument" << std::endl;
			break;

		}
}
/*
/// Register a list contructor.
void AngelScriptEngine::registerListConstructor(const char *obj, const char *declaration, const asSFuncPtr &funcPointer)
{
	int r = engine->RegisterObjectBehaviour(obj, asBEHAVE_LIST_CONSTRUCT, declaration, funcPointer, asCALL_CDECL_OBJLAST);
	assert( r >= 0 );
}
*/
/// Register a property of an object.
/// for byteOffset, use asOFFSET(ClassName, propertyName).
void AngelScriptEngine::registerObjectProperty(const char *obj, const char *declaration, int byteOffset)
{
	int r = engine->RegisterObjectProperty(obj, declaration, byteOffset);
	assert( r >= 0 );
}

void AngelScriptEngine::registerObjectMethod(const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv )
{
	int r = engine->RegisterObjectMethod(obj, declaration, funcPointer, callConv);
	if(r < 0)
		switch(r)
		{
		case asWRONG_CONFIG_GROUP:
			cout << "Wrong config group" << std::endl;
			break;
		case asNOT_SUPPORTED:
			cout << "Not supported" << std::endl;
			break;
		case asINVALID_TYPE:
			cout << "Invalid type" << std::endl;
			break;
		case asINVALID_DECLARATION:
			cout << "Invalid declaration" << std::endl;
			break;
		case asNAME_TAKEN:
			cout << "Name taken" << std::endl;
			break;
		case asWRONG_CALLING_CONV:
			cout << "Wrong calling convention" << std::endl;
			break;
		case asALREADY_REGISTERED:
			cout << "Already registered" << std::endl;
			break;
		}
}

void AngelScriptEngine::registerTypedef(const char * type, const char * decl)
{
	int r = engine->RegisterTypedef(type, decl);
	assert(r >= 0);
}




void AngelScriptEngine::runFunction(asIScriptFunction* func)
{
	asIScriptContext* ctx = 0;

	ctx = getContextFromPool();

	ctx->Prepare(func);
	int r = ctx->Execute();

	if(r != asEXECUTION_FINISHED)
	{
		if(r == asEXECUTION_EXCEPTION)
		{
			string warning = "AngelScript engine says \"";
			warning += ctx->GetExceptionString();
			warning += "\".";
			logWarning(warning);
			return;
		}
	}

	_returnObject = ctx->GetReturnObject();	/// Save the return object (which may not exist)
	_returnValue = ctx->GetReturnQWord();

	returnContextToPool(ctx);
}

void AngelScriptEngine::runFunction(const char * declaration)
{
	asIScriptFunction* func = mod->GetFunctionByDecl(declaration);
	if (func == 0)
	{
		// The function couldn't be found. Instruct the script writer to include the expected function in the script.
		std::cout << "Could not find function \"" << declaration << "\"." << std::endl;
		return;
	}
	runFunction(func);
}

void AngelScriptEngine::runMethod(void* object, asIScriptFunction* func)
{
	asIScriptContext* ctx = 0;

	ctx = getContextFromPool();

	ctx->Prepare(func);

	ctx->SetObject(object);

	int r = ctx->Execute();

	if(r != asEXECUTION_FINISHED)
	{
		if(r == asEXECUTION_EXCEPTION)
		{
			string warning = "AngelScript engine says \"";
			warning += ctx->GetExceptionString();
			warning += "\".";
			logWarning(warning);
			return;
		}
	}

	_returnObject = ctx->GetReturnObject();	/// Save the return object (which may not exist)
	_returnValue = ctx->GetReturnQWord();

	returnContextToPool(ctx);
}

void AngelScriptEngine::runMethod(void* object, const char* declaration)
{
	asIScriptFunction *func = mod->GetFunctionByDecl(declaration);
	if (func == 0)
	{
		// The function couldn't be found. Instruct the script writer to include the expected function in the script.
		std::cout << "could not find function \"" << declaration << "\"." << std::endl;
		return;
	}
	else
		runMethod(object, func);
}



int AngelScriptEngine::getReturnInt()
{
	return _returnValue;
}
float AngelScriptEngine::getReturnFloat()
{
	return *(float*)&_returnValue;
}
double AngelScriptEngine::getReturnDouble()
{
	return *(double*)&_returnValue;
}
bool AngelScriptEngine::getReturnBool()
{
	return (_returnValue != 0);
}
/// NOTE: Must make a new pointer to the object, because the one held by the context will be invalidated.
void* AngelScriptEngine::getReturnObject()
{
	return _returnObject;
}




AngelScriptEngine* getScriptEngine()
{
	static AngelScriptEngine* engine = new AngelScriptEngine();
	return engine;
}

