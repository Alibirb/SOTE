#ifndef ANGELSCRIPTENGINE_H
#define ANGELSCRIPTENGINE_H

#include <iostream>		// string is not included in osgWidget/ScriptEngine
#include <osgWidget/ScriptEngine>


#include <assert.h>  // assert()
#include <string.h>  // strstr()
#ifdef __linux__
#include <sys/time.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#else
#include <conio.h>   // kbhit(), getch()
#include <windows.h> // timeGetTime()
#endif

#include <angelscript.h>
#include "add_on/scriptbuilder/scriptbuilder.h"
#include "add_on/scripthelper/scripthelper.h"


#ifdef __linux__

#define UINT unsigned int
typedef unsigned int DWORD;

// Linux doesn't have timeGetTime(), this essentially does the same thing, except this is milliseconds since Epoch (Jan 1st 1970) instead of system start. It will work the same though...
DWORD timeGetTime();

#endif


#include "globals.h"

using namespace std;

void print(std::string& msg);

void MessageCallback(const asSMessageInfo *msg, void *param);

/// AngelScript engine-y-stuff.
/// Should probably not inherit from osgWidget::ScriptEngine. That class is meant for attaching a script engine to the GUI system.
class AngelScriptEngine : public osgWidget::ScriptEngine
{
protected:
	asIScriptEngine* engine;
	asIScriptContext* ctx;
	asIScriptModule* mod;
	CScriptBuilder builder;

public:
	AngelScriptEngine();
	virtual ~AngelScriptEngine();

	virtual bool initialize();

	virtual bool close();

	virtual bool eval(const std::string& code);

	/// Loads a file and runs the run() function
	virtual bool runFile(const std::string& filePath);

	void registerFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv = asCALL_CDECL)
	{
		int r = engine->RegisterGlobalFunction(declaration, funcPointer, callConv);
		if(r < 0)
			logError("Could not register function.");
	}

	/// Register a global variable. Note that you must pass in a pointer to whatever you want. Even if it's already a pointer.
	void registerProperty(const char *declaration, void *pointer)
	{
		int r = engine->RegisterGlobalProperty(declaration, pointer);
		if(r < 0)
			logError("Could not register property.");
	}

	/// Registers an object type.
	/// See http://www.angelcode.com/angelscript/sdk/docs/manual/doc_register_val_type.html for explanation of how it works.
	/// Use GetTypeTraits<type>() to automatically determine the appropriate traits. (With C++11 compiler).
	void registerObjectType(const char *obj, int byteSize, asDWORD flags, const asSFuncPtr &constructorPointer, const asSFuncPtr &destructorPointer)
	{
		int r = engine->RegisterObjectType(obj, byteSize, flags);
		if(r < 0)
			logError("Could not register object type.");

		r = engine->RegisterObjectBehaviour(obj, asBEHAVE_CONSTRUCT, "void f()", constructorPointer, asCALL_CDECL_OBJLAST);
		assert( r >= 0 );
		r = engine->RegisterObjectBehaviour(obj, asBEHAVE_DESTRUCT, "void f()", destructorPointer, asCALL_CDECL_OBJLAST);
		assert( r >= 0 );
	}

	void registerConstructor(const char *obj, const char *declaration, const asSFuncPtr &funcPointer)
	{
		int r = engine->RegisterObjectBehaviour(obj, asBEHAVE_CONSTRUCT, declaration, funcPointer, asCALL_CDECL_OBJLAST);
		assert( r >= 0 );
	}

	/// Register a list contructor.
	void registerListConstructor(const char *obj, const char *declaration, const asSFuncPtr &funcPointer)
	{
		int r = engine->RegisterObjectBehaviour(obj, asBEHAVE_LIST_CONSTRUCT, declaration, funcPointer, asCALL_CDECL_OBJLAST);
		assert( r >= 0 );
	}

	/// Register a property of an object.
	/// for byteOffset, use asOFFSET(ClassName, propertyName).
	void registerObjectProperty(const char *obj, const char *declaration, int byteOffset)
	{
		int r = engine->RegisterObjectProperty(obj, declaration, byteOffset);
		assert( r >= 0 );
	}

	/// Register a method of an object
	void registerObjectMethod(const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv )
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



	void setArguments(int index)
	{
		// base condition, out of arguments.
	}

	template<typename... Targs>
	void setArguments(int index, std::string nextArgType, double nextArg, Targs... Fargs)
	{
		if(nextArgType == "int")
			if(nextArg < 0)
				logError("Cannot pass negative integers to AngelScript.");
			else
				ctx->SetArgQWord(index, nextArg);
		else if(nextArgType == "float")
			ctx->SetArgFloat(index, nextArg);
		else if(nextArgType == "double")
			ctx->SetArgDouble(index, nextArg);
		else if(nextArgType == "bool")
			ctx->SetArgByte(index, nextArg);
		else
			logError("Unrecognized argument type.");
		setArguments(index+1, Fargs...);
	}

	template<typename... Targs>
	void setArguments(int index, std::string nextArgType, void* nextArg, Targs... Fargs)
	{
		if(nextArgType == "primitiveReference")
			ctx->SetArgAddress(index, nextArg);
		else if(nextArgType == "object")
			ctx->SetArgObject(index, nextArg);
		else
			logError("Unrecognized argument type.");
		setArguments(index+1, Fargs...);
	}


	template<typename... Targs>
	void runFunction(const char * declaration, Targs... Fargs)
	{
		asIScriptFunction *func = mod->GetFunctionByDecl(declaration);
		if (func == 0)
		{
			// The function couldn't be found. Instruct the script writer to include the expected function in the script.
			std::cout << "could not find function \"" << declaration << "\"." << std::endl;
			return;
		}
		ctx->Prepare(func);

		// Set arguments
		setArguments(0, Fargs...);
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
	}

	void runFunction(const char * declaration)
	{
		asIScriptFunction *func = mod->GetFunctionByDecl(declaration);
		if (func == 0)
		{
			// The function couldn't be found. Instruct the script writer to include the expected function in the script.
			std::cout << "could not find function \"" << declaration << "\"." << std::endl;
			return;
		}
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
	}

	int getReturnInt()
	{
		return ctx->GetReturnQWord();
	}
	float getReturnFloat()
	{
		return ctx->GetReturnFloat();
	}
	double getReturnDouble()
	{
		return ctx->GetReturnDouble();
	}
	bool getReturnBool()
	{
		return (ctx->GetReturnByte() != 0);
	}
	/// NOTE: Must make a new pointer to the object, because the one held by the context will be invalidated.
	void* getReturnObject()
	{
		return ctx->GetReturnObject();
	}

	void test();


protected:
	void registerDefaultStuff();
	void registerTestingStuff();

private:
};

AngelScriptEngine* getScriptEngine();


#endif // ANGELSCRIPTENGINE_H
