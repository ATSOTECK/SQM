//
// Created by Skyler on 11/23/21.
//

#include "qubit.hpp"

#include <cmath>
#include <iostream>
#include <cstdarg>
#include <random>

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

Qubit Qubit::zero(int n) {
    if (n <= 0) {
        n = 1;
    }

    int amount = std::pow(2, n);

    Qubit ret;
    ret._size = n;
    ret._components.emplace_back(1, 0);

    for (int i = 0; i < amount - 1; ++i) {
        ret._components.emplace_back(0, 0);
    }

    return ret;
}

Qubit Qubit::one(int n) {
    if (n <= 0) {
        n = 1;
    }

    int amount = std::pow(2, n);

    Qubit ret;
    ret._size = n;
    ret._components.emplace_back(0, 0);
    ret._components.emplace_back(1, 0);

    for (int i = 0; i < amount - 2; ++i) {
        ret._components.emplace_back(0, 0);
    }

    return ret;
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

std::ostream &operator <<(std::ostream &os, const Qubit &q) {
    os << "qubits: " << q._size << std::endl;
    os << "components: [real, imag]" << std::endl;

    for (const auto &c : q._components) {
        os << "[" << c.real() << ", " << c.imag() << "]" << std::endl;
    }

    return os;
}
