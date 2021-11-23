#include <iostream>

#include "src/common.hpp"
#include "src/gates.hpp"
#include "src/qubit.hpp"

int main() {
    Qubit qubit;
    qubit.zero(2);

    /*Qubit a = Qubit(1, ONE_OVER_SQRT2, 0, ONE_OVER_SQRT2, 0);
    a.print();
    std::cout << a.isValid() << std::endl;*/

    db(qubit);
    hadamard(qubit);
    zGate(qubit);
    db(qubit);

    db("valid? " << (qubit.isValid() ? "yes" : "no"));
    db("measure: " << qubit.measure());

    return 0;
}
