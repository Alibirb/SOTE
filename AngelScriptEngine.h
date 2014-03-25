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
#include "scriptbuilder/scriptbuilder.h"
#include "scripthelper/scripthelper.h"


#ifdef __linux__

	#define UINT unsigned int
	typedef unsigned int DWORD;

	// Linux doesn't have timeGetTime(), this essentially does the same thing, except this is milliseconds since Epoch (Jan 1st 1970) instead of system start. It will work the same though...
	DWORD timeGetTime();

#endif


#include "globals.h"

#include "stdvector.h"

#include "scriptstl.h"

using namespace std;

void print(std::string& msg);

void MessageCallback(const asSMessageInfo* msg, void* param);

/// AngelScript engine-y-stuff.
/// Should probably not inherit from osgWidget::ScriptEngine. That class is meant for attaching a script engine to the GUI system.
class AngelScriptEngine : public osgWidget::ScriptEngine
{
protected:
	asIScriptEngine* engine;
	asIScriptModule* mod;
	CScriptBuilder builder;
	bool executing = false;	/// used to detect nested calls
	long long int _returnValue;
	void* _returnObject;

	std::vector<asIScriptContext*> pool;

	asIScriptContext* getContextFromPool();

	void returnContextToPool(asIScriptContext* ctx);

	void setArguments(asIScriptContext* ctx, int index)
	{
		// base condition, out of arguments.
	}

	template<typename... Targs>
	void setArguments(asIScriptContext* ctx, int index, std::string nextArgType, double nextArg, Targs... Fargs)
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
		setArguments(ctx, index+1, Fargs...);
	}

	template<typename... Targs>
	void setArguments(asIScriptContext* ctx, int index, std::string nextArgType, void* nextArg, Targs... Fargs)
	{
		if(nextArgType == "primitiveReference")
			ctx->SetArgAddress(index, nextArg);
		else if(nextArgType == "object")
			ctx->SetArgObject(index, nextArg);
		else
			logError("Unrecognized argument type.");
		setArguments(ctx, index+1, Fargs...);
	}


public:

	AngelScriptEngine();
	virtual ~AngelScriptEngine();

	virtual bool initialize();

	virtual bool close();

	virtual bool eval(const std::string& code);

	/// Loads a file and runs the specified function
	virtual bool runFile(const std::string& filePath, const char* functionName="void run()");


	/// Compiles a function.
	void addFunction(const char* sectionName, const char* code, int lineOffset, asDWORD compileFlags, asIScriptFunction** outFunc);

	/// Compiles and returns the function.
	/// TODO: Do I really need this and addFunction?
	asIScriptFunction* compileFunction(const char* sectionName, const char* code, int lineOffset, asDWORD compileFlags);

	void registerFunction(const char* declaration, const asSFuncPtr &funcPointer, asDWORD callConv = asCALL_CDECL);

	/// Register a global variable. Note that you must pass in a pointer to whatever you want. Even if it's already a pointer.
	void registerProperty(const char* declaration, void* pointer);

	/// Registers an object type.
	/// See http://www.angelcode.com/angelscript/sdk/docs/manual/doc_register_val_type.html for explanation of how it works.
	/// Use GetTypeTraits<type>() to automatically determine the appropriate traits. (With C++11 compiler).
	void registerObjectType(const char* obj, int byteSize, asDWORD flags);

	/// Registers an object type, along with default constructor and destructor.
	/// See http://www.angelcode.com/angelscript/sdk/docs/manual/doc_register_val_type.html for explanation of how it works.
	/// Use GetTypeTraits<type>() to automatically determine the appropriate traits. (With C++11 compiler).
	void registerObjectType(const char* obj, int byteSize, asDWORD flags, const asSFuncPtr &constructorPointer, const asSFuncPtr &destructorPointer);

	/// Registers a factory function. For use by reference types.
	void registerFactoryFunction(const char* obj, const char* declaration, const asSFuncPtr &funcPointer);

	void registerDestructor(const char* obj, const asSFuncPtr &funcPointer);

	/// Registers a constructor. For use by value types.
	void registerConstructor(const char* obj, const char* declaration, const asSFuncPtr &funcPointer);

	/// Register a list contructor.
//	void registerListConstructor(const char* obj, const char* declaration, const asSFuncPtr &funcPointer);

	/// Register a property of an object.
	/// for byteOffset, use asOFFSET(ClassName, propertyName).
	void registerObjectProperty(const char* obj, const char* declaration, int byteOffset);

	/// Register a method of an object
	void registerObjectMethod(const char* obj, const char* declaration, const asSFuncPtr &funcPointer, asDWORD callConv );

	/// Register an std::vector type.
	/// vectorName is the name of the new vector type
	/// typeName is the name of the type that the vector holds
	template<typename T>
	void registerVector(std::string vectorName, std::string typeName)
	{
		RegisterVector<T>(vectorName, typeName, engine);
	}

	template<typename T>
	void registerListType(std::string listName, std::string typeName)
	{
		RegisterScriptList<T>(listName.c_str(), typeName.c_str(), engine);
	}



	void registerEnumValues(const char* enumType)
	{
		// Base condition. Out of values
	}

	template<typename... Targs>
	void registerEnumValues(const char* enumType, const char*  valueName, int value, Targs... Fargs)
	{
		int r = engine->RegisterEnumValue(enumType, valueName, value);
		assert(r >= 0);
		registerEnumValues(enumType, Fargs...);
	}


	template<typename... Targs>
	void registerEnum(const char*  type, Targs... Fargs)
	{
		int r = engine->RegisterEnum(type);
		assert(r >= 0);
		registerEnumValues(type, Fargs...);
	}

	/// Registers a typedef. Currently only works for built-in primitive types.
	void registerTypedef(const char*  type, const char*  decl);



	void runFunction(asIScriptFunction* func);

	template<typename... Targs>
	void runFunction(asIScriptFunction* func, Targs... Fargs)
	{
		asIScriptContext* ctx = 0;

		ctx = getContextFromPool();

		ctx->Prepare(func);
		setArguments(ctx, 0, Fargs...);
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


	template<typename... Targs>
	void runFunction(const char* declaration, Targs... Fargs)
	{
		asIScriptFunction* func = mod->GetFunctionByDecl(declaration);
		if (func == 0)
		{
			// The function couldn't be found. Instruct the script writer to include the expected function in the script.
			std::cout << "could not find function \"" << declaration << "\"." << std::endl;
			return;
		}
		runFunction(func, Fargs...);
	}

	void runFunction(const char* declaration);

	template<typename... Targs>
	void runMethod(void* object, asIScriptFunction* func, Targs... Fargs)
	{
		asIScriptContext* ctx = 0;

		ctx = getContextFromPool();

		ctx->Prepare(func);
		executing = true;
		setArguments(ctx, 0, Fargs...);

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

	void runMethod(void* object, asIScriptFunction* func);

	template<typename... Targs>
	void runMethod(void* object, const char* declaration, Targs... Fargs)
	{
		asIScriptFunction* func = mod->GetFunctionByDecl(declaration);
		if (func == 0)
		{
			// The function couldn't be found. Instruct the script writer to include the expected function in the script.
			std::cout << "could not find function \"" << declaration << "\"." << std::endl;
			return;
		}
		else
			runMethod(object, func, Fargs...);
	}

	void runMethod(void* object, const char* declaration);


	int getReturnInt();
	float getReturnFloat();
	double getReturnDouble();
	bool getReturnBool();
	/// NOTE: Must make a new pointer to the object, because the one held by the context will be invalidated.
	void* getReturnObject();

	void test();


protected:
	void registerVec3();
	void registerDefaultStuff();
	void registerTestingStuff();
};


AngelScriptEngine* getScriptEngine();


#endif // ANGELSCRIPTENGINE_H
