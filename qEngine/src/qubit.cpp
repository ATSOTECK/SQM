//
// Created by Skyler on 11/23/21.
//

#include "qubit.hpp"

#include <bitset>
#include <cmath>
#include <iostream>
#include <cstdarg>
#include <random>

void Qubit::Register(asIScriptEngine *engine) {
    int r;
    r = engine->RegisterEnum("Kind"); assert(r >= 0);
    r = engine->RegisterEnumValue("Kind", "Zero", 0); assert(r >= 0);
    r = engine->RegisterEnumValue("Kind", "One", 1); assert(r >= 0);

    r = engine->RegisterObjectType("Qubit", sizeof(Qubit), asOBJ_REF); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Qubit", asBEHAVE_FACTORY, "Qubit@ f()", asFUNCTION(refFactoryA), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Qubit", asBEHAVE_FACTORY, "Qubit@ f(Kind kind, int n)", asFUNCTION(refFactoryB), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Qubit", asBEHAVE_ADDREF, "void f()", asMETHOD(Qubit, addRef), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Qubit", asBEHAVE_RELEASE, "void f()", asMETHOD(Qubit, release), asCALL_THISCALL); assert(r >= 0);
    
    r = engine->RegisterObjectMethod("Qubit", "void zero(int n)", asMETHODPR(Qubit, zero, (int), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "void one(int n)", asMETHODPR(Qubit, one, (int), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "bool isValid()", asMETHODPR(Qubit, isValid, (void)const, bool), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "int size()", asMETHODPR(Qubit, size, (void)const, int), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "int count()", asMETHODPR(Qubit, count, (void)const, int), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "int measure()", asMETHODPR(Qubit, measure, (void)const, int), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "double magnitude()", asMETHODPR(Qubit, magnitude, (void)const, double), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "string toString()", asMETHODPR(Qubit, toString, (void)const, std::string), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("Qubit", "void showProbabilities()", asMETHODPR(Qubit, showProbabilities, (void)const, void), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("void print(const Qubit &in)", asFUNCTION(printQubit), asCALL_CDECL); assert(r >= 0);
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
    return (std::abs(1.0 - magnitude()) < 1.0e-8);
}

bool Qubit::isOne() const {
    return (_components[1].real() == 1 && isValid());
}

int Qubit::size() const {
    return _size;
}

int Qubit::count() const {
    return static_cast<int>(_components.size());
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

Component Qubit::component(int index) const {
    if (index >= _components.size() || index < 0) {
        return Component(0, 0);
    }
    
    return _components[index];
}

void Qubit::setComponent(int index, Component component) {
    if (index >= _components.size() || index < 0) {
        return;
    }
    
    _components[index] = component;
}

std::vector<double> Qubit::probabilities() const {
    std::vector<double> probabilities;
    for (const auto &c : _components) {
        double p = std::pow(c.real(), 2) + std::pow(c.imag(), 2);
        probabilities.push_back(p);
    }

    return probabilities;
}

int Qubit::measure() const {
    std::vector<double> prob = std::move(probabilities());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution(prob.begin(), prob.end());

    return distribution(gen);
}

double Qubit::magnitude() const {
    double val = 0;
    
    for (const auto &c : _components) {
        val += std::pow(c.real(), 2) + std::pow(c.imag(), 2);
    }
    
    return val;
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

void Qubit::showProbabilities() const {
    db("probability distribution:");
    dbnln("[ ");
    for (const auto &p : probabilities()) {
        dbnln(p * 100 << "% ");
    }
    db("]");
}

void Qubit::addRef() {
    ++_refCount;
}

void Qubit::release() {
    if (--_refCount == 0) {
        delete this;
    }
}

std::ostream &operator <<(std::ostream &os, const Qubit &q) {
    os << "qubits: " << q._size << std::endl;
    os << "components:" << std::endl;

    int idx = 0;
    for (const auto &c : q._components) {
        if (c.imag() == 0 || c.imag() == -0) {
            os << "[" << c.real() << ", " << c.imag() << "]";
        } else {
            os << "[" << c.real() << ", " << c.imag() << "i]";
        }
    
        std::string bin = std::bitset<64>(idx++).to_string();
        bin = bin.substr(bin.length() - q.size());
        
        os << " |" << bin << ">" << std::endl;
    }

    return os;
}

Qubit *refFactoryA() {
    return new Qubit();
}

Qubit *refFactoryB(Qubit::Kind kind, int n) {
    return new Qubit(kind, n);
}

void printQubit(const Qubit &q) {
    db(q);
}
