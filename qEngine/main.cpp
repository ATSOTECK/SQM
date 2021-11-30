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

void println(const std::string &str) {
    db(str << std::endl);
}

asIScriptEngine *initAngelScript() {
    asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    int r;
    r = engine->SetMessageCallback(asFUNCTION(messageCallback), nullptr, asCALL_CDECL); assert(r >= 0);
    RegisterStdString(engine);
    RegisterScriptHandle(engine);
    RegisterScriptWeakRef(engine);
    
    r = engine->RegisterObjectType("size_t", sizeof(size_t), asOBJ_VALUE | asOBJ_POD); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void println(const string &in)", asFUNCTION(println), asCALL_CDECL); assert( r >= 0 );

    Qubit::Register(engine);
    registerGates(engine);

    return engine;
}

int main() {
    asIScriptEngine *engine = initAngelScript();
    CScriptBuilder builer;
    int r = builer.StartNewModule(engine, "SQM");
    if (r < 0) {
        db("Error making module. Probably no memory.");
        exit(0);
    }

    r = builer.AddSectionFromFile("../scripts/tst.sqm");
    if (r < 0) {
        db("Could not load file.");
        exit(0);
    }

    r = builer.BuildModule();
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
    engine->ShutDownAndRelease();

    return 0;

    Qubit qubit(Qubit::Zero, 4);

    /*Qubit a = Qubit(1, ONE_OVER_SQRT2, 0, ONE_OVER_SQRT2, 0);
    a.print();
    std::cout << a.isValid() << std::endl;*/

    db(qubit);
    hadamard(qubit);
    zGate(qubit);
    db(qubit);

    db("valid? " << (qubit.isValid() ? "yes" : "no"));
    db("measure: " << qubit.measure());

    return 0;
}
