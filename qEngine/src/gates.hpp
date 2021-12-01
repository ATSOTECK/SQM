//
// Created by Skyler on 11/23/21.
//

#ifndef QENGINE_GATES_HPP
#define QENGINE_GATES_HPP

#include <angelscript.h>

#include "qubit.hpp"

void hadamard(Qubit &q);
void xGate(Qubit &q);
void yGate(Qubit &q);
void zGate(Qubit &q);

void sqrtX(Qubit &q);

void sGate(Qubit &q);
void tGate(Qubit &q);

void cnot(const Qubit &x, Qubit &y);

void registerGates(asIScriptEngine *engine);

#endif //QENGINE_GATES_HPP
