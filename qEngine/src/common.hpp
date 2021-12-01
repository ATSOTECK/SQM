//
// Created by Skyler on 11/23/21.
//

#ifndef QENGINE_COMMON_HPP
#define QENGINE_COMMON_HPP

#define SQRT2 1.41421356237
#define ONE_OVER_SQRT2 0.70710678118
#define E_TO_IPI_OVER_4 std::complex<double>(ONE_OVER_SQRT2, ONE_OVER_SQRT2)
#define E_TO_MINUS_IPI_OVER_4 std::complex<double>(ONE_OVER_SQRT2, -ONE_OVER_SQRT2)

#include <iostream>
#define db(x) std::cout << x << std::endl
#define dbnln(x) std::cout << x

#endif //QENGINE_COMMON_HPP
