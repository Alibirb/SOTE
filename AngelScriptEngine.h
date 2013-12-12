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
#include "add_on/scriptstdstring/scriptstdstring.h"
#include "add_on/scriptbuilder/scriptbuilder.h"
#include "add_on/scriptstdstring/scriptstdstring.h"
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

protected:
private:
};

AngelScriptEngine* getScriptEngine();


#endif // ANGELSCRIPTENGINE_H
