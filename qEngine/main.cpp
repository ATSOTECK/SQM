#include <iostream>
#include <Eigen/Dense>

using Eigen::MatrixXd;

#include "src/common.hpp"
#include "src/gates.hpp"
#include "src/qubit.hpp"

int main() {
    /*
    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << std::endl;*/

    Qubit qubit = Qubit::zero(4);
    //qubit.print();
    //std::cout << qubit.isValid() << std::endl;

    /*Qubit a = Qubit(1, ONE_OVER_SQRT2, 0, ONE_OVER_SQRT2, 0);
    a.print();
    std::cout << a.isValid() << std::endl;*/

    db(qubit);
    //hadamard(qubit);
    hadamard(qubit);
    yGate(qubit);
    db(qubit);

    db("valid? " << (qubit.isValid() ? "yes" : "no"));
    db("measure: " << qubit.measure());

    return 0;
}
