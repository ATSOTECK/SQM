#include <iostream>

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scripthandle/scripthandle.h>
#include <weakref/weakref.h>

#include "src/common.hpp"
#include "src/gates.hpp"
#include "src/qubit.hpp"

std::string nameFromPath(std::string path) {
    std::string name, tmp;

    for (int i = (int)path.length() - 1; i >= 0; i--) {
        if (path[i] != '/' && path[i] != '\\') {
            tmp += path[i];
        } else {
            break;
        }
    }

    for (int j = (int)tmp.length() - 1; j >= 0; j--) {
        name += tmp[j];
    }

    return name;
}

void messageCallback(const asSMessageInfo &msg) {
    const char *type = "ERROR";
    if (msg.type == asMSGTYPE_INFORMATION) {
        type = "INFO";
    } else if (msg.type == asMSGTYPE_WARNING) {
        type = "WARNING";
    }

    //fprintf(stderr ,"%s (%d, %d) : %s : %s\n", msg.section, msg.row, msg.col, type, msg.message);

    std::stringstream str;
    str << nameFromPath(msg.section) << " (" << msg.row << ", " << msg.col << ") : " << type << " : " << msg.message << "\n";
    db(str.str());
}

void printlnStr(const std::string &str) {
    db(str);
}

void printlnDouble(double n) {
    db(n);
}

asIScriptEngine *initAngelScript() {
    asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    int r;
    r = engine->SetMessageCallback(asFUNCTION(messageCallback), nullptr, asCALL_CDECL); assert(r >= 0);
    RegisterStdString(engine);
    RegisterScriptHandle(engine);
    RegisterScriptWeakRef(engine);
    
    r = engine->RegisterObjectType("size_t", sizeof(size_t), asOBJ_VALUE | asOBJ_POD); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void println(const string &in)", asFUNCTION(printlnStr), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void println(double)", asFUNCTION(printlnDouble), asCALL_CDECL); assert( r >= 0 );

    Qubit::Register(engine);
    registerGates(engine);

    return engine;
}

void run(asIScriptEngine *engine) {
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, "SQM");
    if (r < 0) {
        db("Error making module. Probably no memory.");
        exit(0);
    }
    
    r = builder.AddSectionFromFile("../scripts/tst.sqm");
    if (r < 0) {
        db("Could not load file.");
        exit(0);
    }
    
    r = builder.BuildModule();
    if (r < 0) {
        db("Errors.");
        exit(0);
    }
    
    asIScriptModule *mod = engine->GetModule("SQM");
    asIScriptFunction *func = mod->GetFunctionByDecl("void run()");
    
    if (func == nullptr) {
        db("The function must have 'void run()' to run.");
        exit(0);
    }
    
    asIScriptContext *ctx = engine->CreateContext();
    ctx->Prepare(func);
    r = ctx->Execute();
    
    if (r == asEXECUTION_EXCEPTION) {
        db("Exception: " << ctx->GetExceptionString());
    }
    
    ctx->Release();
}

int main() {
    asIScriptEngine *engine = initAngelScript();
    run(engine);
    
    engine->ShutDownAndRelease();

    return 0;
}
