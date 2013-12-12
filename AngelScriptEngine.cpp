#include "AngelScriptEngine.h"


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


void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING )
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";

	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}


AngelScriptEngine::AngelScriptEngine()
{
	//ctor
}

AngelScriptEngine::~AngelScriptEngine()
{
	//dtor
}


bool AngelScriptEngine::initialize()
{
	// Create the script engine.
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	// Set the message callback to receive information on errors in human readable form.
	int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	assert(r >= 0);

	// Register the standard string type.
	// There's no definitive standard string type for C++, so AngelScript doesn't have a built-in string type. The application must define its own.
	RegisterStdString(engine);

	// Register the function that we want the scripts to call
	r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
	assert(r >= 0);

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
	int r = ExecuteString(engine, code.c_str(), mod, ctx);
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


bool AngelScriptEngine::runFile(const std::string& filePath)
{
	int r = builder.StartNewModule(engine, "MyModule");
	if (r < 0 )
	{
		// If the code fails here it is usually because there is more memory to allocate the module.
		std::cout << "Unrecoverable error while starting a new module." << std::endl;
		return false;
	}
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

	// Find the function that is to be called.
	mod = engine->GetModule("MyModule");
	asIScriptFunction *func = mod->GetFunctionByDecl("void run()");
	if (func == 0)
	{
		// The function couldn't be found. Instruct the script writer to include the expected function in the script.
		std::cout << "The script \"" << filePath << "\" must have the function 'void run()'." << std::endl;
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

	return true;
}


AngelScriptEngine* getScriptEngine()
{
	static AngelScriptEngine* engine = new AngelScriptEngine();
	return engine;
}
