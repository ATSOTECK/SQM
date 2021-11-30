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
    [[nodiscard]] int size() const;
    [[nodiscard]] size_t count() const;

    [[nodiscard]] VectorXcd toVector() const;
    void update(const VectorXcd &v);

    [[nodiscard]] std::vector<double> probabilities() const;
    [[nodiscard]] int measure() const;

    [[nodiscard]] std::string toString() const;

    //void operator=(const Qubit &) = delete;

private:
    friend std::ostream &operator <<(std::ostream &os, const Qubit &q);

    int _size{};
    std::vector<Component> _components;
};


void qubitConstructorA(void *mem);
void qubitConstructorB(Qubit::Kind kind, int n, void *mem);

void printQubit(const Qubit &q);

#endif //QENGINE_QUBIT_HPP
