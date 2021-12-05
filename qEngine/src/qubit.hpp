//
// Created by Skyler on 11/23/21.
//

#ifndef QENGINE_QUBIT_HPP
#define QENGINE_QUBIT_HPP

#include <angelscript.h>
#include <complex>
#include <vector>
#include <Eigen/Dense>

#include "common.hpp"

using Eigen::VectorXcd;

typedef std::complex<double> Component;

class Qubit {
public:
    enum Kind {
        Zero,
        One
    };

    static void Register(asIScriptEngine *engine);

    Qubit(const Qubit &) = delete;
    Qubit() = default;
    Qubit(Kind kind, int n);
    Qubit(int n, double component, ...);

    void zero(int n);
    void one(int n);

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isOne() const;
    [[nodiscard]] int size() const;
    [[nodiscard]] int count() const;

    [[nodiscard]] VectorXcd toVector() const;
    void update(const VectorXcd &v);
    
    [[nodiscard]] Component component(int index) const;
    void setComponent(int index, Component component);

    [[nodiscard]] std::vector<double> probabilities() const;
    [[nodiscard]] int measure() const;
    [[nodiscard]] double magnitude() const;

    [[nodiscard]] std::string toString() const;
    void showProbabilities() const;
    
    void addRef();
    void release();

    void operator=(const Qubit &) = delete;

private:
    friend std::ostream &operator <<(std::ostream &os, const Qubit &q);

    int _refCount{1};
    int _size{};
    std::vector<Component> _components;
};

Qubit *refFactoryA();
Qubit *refFactoryB(Qubit::Kind kind, int n);

void printQubit(const Qubit &q);

#endif //QENGINE_QUBIT_HPP
