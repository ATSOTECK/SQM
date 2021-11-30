cmake_minimum_required(VERSION 3.17)
project(qEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)
include_directories(inc/angelscript)
link_directories(${PROJECT_SOURCE_DIR}/lib)

enable_language(ASM_MASM)

set(ANGELSCRIPT_HEADERS
        src/angelscript/as_array.h
        src/angelscript/as_builder.h
        src/angelscript/as_bytecode.h
        src/angelscript/as_callfunc.h
        src/angelscript/as_compiler.h
        src/angelscript/as_config.h
        src/angelscript/as_configgroup.h
        src/angelscript/as_context.h
        src/angelscript/as_criticalsection.h
        src/angelscript/as_datatype.h
        src/angelscript/as_debug.h
        src/angelscript/as_generic.h
        src/angelscript/as_map.h
        src/angelscript/as_memory.h
        src/angelscript/as_module.h
        src/angelscript/as_objecttype.h
        src/angelscript/as_outputbuffer.h
        src/angelscript/as_parser.h
        src/angelscript/as_property.h
        src/angelscript/as_restore.h
        src/angelscript/as_scriptcode.h
        src/angelscript/as_scriptengine.h
        src/angelscript/as_scriptfunction.h
        src/angelscript/as_scriptnode.h
        src/angelscript/as_scriptobject.h
        src/angelscript/as_string.h
        src/angelscript/as_string_util.h
        src/angelscript/as_texts.h
        src/angelscript/as_thread.h
        src/angelscript/as_tokendef.h
        src/angelscript/as_tokenizer.h
        src/angelscript/as_typeinfo.h
        src/angelscript/as_variablescope.h
        )

set(ANGELSCRIPT_SOURCE
        src/angelscript/as_atomic.cpp
        src/angelscript/as_builder.cpp
        src/angelscript/as_bytecode.cpp
        src/angelscript/as_callfunc.cpp
        src/angelscript/as_callfunc_mips.cpp
        src/angelscript/as_callfunc_x86.cpp
        src/angelscript/as_callfunc_x64_gcc.cpp
        src/angelscript/as_callfunc_x64_msvc.cpp
        src/angelscript/as_callfunc_x64_mingw.cpp
        src/angelscript/as_compiler.cpp
        src/angelscript/as_configgroup.cpp
        src/angelscript/as_context.cpp
        src/angelscript/as_datatype.cpp
        src/angelscript/as_gc.cpp
        src/angelscript/as_generic.cpp
        src/angelscript/as_globalproperty.cpp
        src/angelscript/as_memory.cpp
        src/angelscript/as_module.cpp
        src/angelscript/as_objecttype.cpp
        src/angelscript/as_outputbuffer.cpp
        src/angelscript/as_parser.cpp
        src/angelscript/as_restore.cpp
        src/angelscript/as_scriptcode.cpp
        src/angelscript/as_scriptengine.cpp
        src/angelscript/as_scriptfunction.cpp
        src/angelscript/as_scriptnode.cpp
        src/angelscript/as_scriptobject.cpp
        src/angelscript/as_string.cpp
        src/angelscript/as_string_util.cpp
        src/angelscript/as_thread.cpp
        src/angelscript/as_tokenizer.cpp
        src/angelscript/as_typeinfo.cpp
        src/angelscript/as_variablescope.cpp
        src/angelscript/as_callfunc_x64_msvc_asm.asm
        )

add_executable(qEngine main.cpp src/qubit.cpp src/qubit.hpp src/common.hpp src/gates.cpp src/gates.hpp
        inc/angelscript/scriptstdstring/scriptstdstring.cpp
        inc/angelscript/scripthandle/scripthandle.cpp
        inc/angelscript/weakref/weakref.cpp
        inc/angelscript/scriptbuilder/scriptbuilder.cpp
        inc/angelscript/scriptarray/scriptarray.cpp
        inc/angelscript/scripthelper/scripthelper.cpp
        ${ANGELSCRIPT_HEADERS}
        ${ANGELSCRIPT_SOURCE}
)