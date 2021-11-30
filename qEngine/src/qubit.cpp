//
// Created by Skyler on 11/23/21.
//

#include "qubit.hpp"

#include <cmath>
#include <iostream>
#include <cstdarg>
#include <random>

void Qubit::Register(asIScriptEngine *engine) {
    int r;
    r = engine->RegisterEnum("Kind"); assert(r >= 0);
    r = engine->RegisterEnumValue("Kind", "Zero", 0); assert(r >= 0);
    r = engine->RegisterEnumValue("Kind", "One", 1); assert(r >= 0);

    r = engine->RegisterObjectType("Qubit", sizeof(Qubit), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Qubit", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(qubitConstructorA), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Qubit", asBEHAVE_CONSTRUCT, "void f(Kind kind, int n)", asFUNCTION(qubitConstructorB), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "void zero(int n)", asMETHODPR(Qubit, zero, (int), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "void one(int n)", asMETHODPR(Qubit, one, (int), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "bool isValid()", asMETHODPR(Qubit, isValid, (void)const, bool), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "int size()", asMETHODPR(Qubit, size, (void)const, int), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "size_t count()", asMETHODPR(Qubit, count, (void)const, size_t), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "int measure()", asMETHODPR(Qubit, measure, (void)const, int), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "string toString()", asMETHODPR(Qubit, toString, (void)const, std::string), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("void print(const Qubit &q)", asFUNCTION(printQubit), asCALL_CDECL); assert(r >= 0);
}

Qubit::Qubit(Kind kind, int n) {
    if (kind == Zero) {
        zero(n);
    } else if (kind == One) {
        one(n);
    }
}

Qubit::Qubit(int n, double component, ...) {
    _size = n;
    int amount = static_cast<int>(std::pow(2, _size)) * 2;
    va_list list;
    va_start(list, component);
    _components.emplace_back(component, va_arg(list, double));

    for (int i = 0; i < amount - 2; i += 2) {
        double a = va_arg(list, double);
        double b = va_arg(list, double);

        _components.emplace_back(a, b);
    }

    va_end(list);
}

void Qubit::zero(int n) {
    if (n <= 0) {
        n = 1;
    }

    int amount = std::pow(2, n);
    _size = n;
    _components.clear();
    _components.emplace_back(1, 0);

    for (int i = 0; i < amount - 1; ++i) {
        _components.emplace_back(0, 0);
    }
}

void Qubit::one(int n) {
    if (n <= 0) {
        n = 1;
    }

    int amount = std::pow(2, n);
    _size = n;
    _components.clear();
    _components.emplace_back(0, 0);
    _components.emplace_back(1, 0);

    for (int i = 0; i < amount - 2; ++i) {
        _components.emplace_back(0, 0);
    }
}

bool Qubit::isValid() const {
    Component val = 0;

    for (const auto &c : _components) {
        val += std::pow(c, 2);
    }

    //db("val: " << std::abs(val.real() + val.imag()));
    return (std::abs(1.0 - std::abs(val.real() + val.imag())) < 0.00001);
}

int Qubit::size() const {
    return _size;
}

size_t Qubit::count() const {
    return _components.size();
}

VectorXcd Qubit::toVector() const {
    size_t cnt = count();
    VectorXcd v(cnt);

    for (int i = 0; i < cnt; ++i) {
        v(i) = _components[i];
    }

    return v;
}

void Qubit::update(const VectorXcd &v) {
    for (int i = 0; i < v.size(); ++i) {
        _components[i] = v[i];
    }
}

std::vector<double> Qubit::probabilities() const {
    std::vector<double> probabilities;
    for (const auto &c : _components) {
        Component s = std::pow(c, 2);
        probabilities.push_back(std::abs(s.real()));
    }

    return probabilities;
}

int Qubit::measure() const {
    std::vector<double> prob = std::move(probabilities());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<double> distribution(prob.begin(), prob.end());

    return distribution(gen);
}

std::string Qubit::toString() const {
    std::stringstream str;

    str << "qubits: " << _size << std::endl;
    str << "components: [real, imag]" << std::endl;

    for (const auto &c : _components) {
        str << "[" << c.real() << ", " << c.imag() << "]" << std::endl;
    }

    return str.str();
}

std::ostream &operator <<(std::ostream &os, const Qubit &q) {
    os << "qubits: " << q._size << std::endl;
    os << "components: [real, imag]" << std::endl;

    for (const auto &c : q._components) {
        os << "[" << c.real() << ", " << c.imag() << "]" << std::endl;
    }

    return os;
}

void qubitConstructorA(void *mem) {
    new(mem) Qubit();
}

void qubitConstructorB(Qubit::Kind kind, int n, void *mem) {
    new(mem) Qubit(kind, n);
}

void printQubit(const Qubit &q) {
    db(q);
}
