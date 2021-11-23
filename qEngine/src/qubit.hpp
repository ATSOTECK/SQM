//
// Created by Skyler on 11/23/21.
//

#ifndef QENGINE_QUBIT_HPP
#define QENGINE_QUBIT_HPP

#include <complex>
#include <vector>
#include <Eigen/Dense>

#include "common.hpp"

using Eigen::VectorXcd;

typedef std::complex<double> Component;

class Qubit {
public:
    Qubit(const Qubit &) = delete;
    Qubit() = default;
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

    void operator=(const Qubit &) = delete;

private:
    friend std::ostream &operator <<(std::ostream &os, const Qubit &q);

    int _size{};
    std::vector<Component> _components;
};


#endif //QENGINE_QUBIT_HPP
