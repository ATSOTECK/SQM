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

//theta is the angle in degrees.
void rxGate(Qubit &q, double theta);
void ryGate(Qubit &q, double theta);
void rzGate(Qubit &q, double theta);

void sqrtX(Qubit &q);

void sGate(Qubit &q);
void tGate(Qubit &q);

//The following functions assume 2 one qubit qubits are passed.
void cnot(const Qubit &x, Qubit &y);
void swap(Qubit &x, Qubit &y);
void sqrtSwap(Qubit &x, Qubit &y);

void registerGates(asIScriptEngine *engine);

#endif //QENGINE_GATES_HPP
