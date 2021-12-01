//
// Created by Skyler on 11/23/21.
//

#include "gates.hpp"

#include "common.hpp"

#include <Eigen/Dense>

using namespace std::complex_literals;

using Eigen::MatrixXd;
using Eigen::MatrixXcd;
using Eigen::VectorXcd;

template<typename T>
T kroneckerProduct(const T &a, const T &b) {
    int arows = a.rows();
    int acols = a.cols();
    int brows = b.rows();
    int bcols = b.cols();
    T ret = T(arows * brows, acols * bcols);
    for (size_t i = 0; i < arows; ++i) {
        for (size_t j = 0; j < acols; ++j) {
            for (size_t k = 0; k < brows; ++k) {
                for (size_t l = 0; l < bcols; ++l) {
                    ret(i * brows + k, j * bcols + l) = a(i, j) * b(k, l);
                }
            }
        }
    }

    return ret;
}

void hadamard(Qubit &q) {
    int n = q.count();

    MatrixXd h = MatrixXd(n, n);
    h(0, 0) = 1;

    for (int i = 1; i < n; i += i) {
        for (int row = 0; row < i; ++row) {
            for (int col = 0; col < i; ++col) {
                h(row + i, col) = h(row, col);
                h(row, col + i) = h(row, col);
                h(row + i, col + i) = -h(row, col);
            }
        }
    }

    VectorXcd v = q.toVector();
    VectorXcd res = (h * v) * std::pow(ONE_OVER_SQRT2, q.size());
    q.update(res);
}

void xGate(Qubit &q) {
    MatrixXd baseX = MatrixXd(2, 2);
    baseX.setZero();
    baseX(0, 1) = 1;
    baseX(1, 0) = 1;
    MatrixXd x = baseX;

    for (int i = 1; i < q.size(); ++i) {
        x = kroneckerProduct(baseX, x);
        //db(i << " x:\n" << x);
    }

    VectorXcd v = q.toVector();
    VectorXcd res = (x * v);
    q.update(res);
}

void yGate(Qubit &q) {
    MatrixXcd baseY = MatrixXd(2, 2);
    baseY.setZero();
    baseY(0, 1) = -1i;
    baseY(1, 0) = 1i;
    MatrixXcd y = baseY;

    for (int i = 1; i < q.size(); ++i) {
        y = kroneckerProduct(baseY, y);
        //db(i << " y:\n" << y);
    }

    VectorXcd v = q.toVector();
    VectorXcd res = (y * v);
    q.update(res);
}

void zGate(Qubit &q) {
    MatrixXd baseZ = MatrixXd(2, 2);
    baseZ.setZero();
    baseZ(0, 0) = 1;
    baseZ(1, 1) = -1;
    MatrixXd z = baseZ;

    for (int i = 1; i < q.size(); ++i) {
        z = kroneckerProduct(baseZ, z);
        //db(i << " z:\n" << z);
    }

    VectorXcd v = q.toVector();
    VectorXcd res = (z * v);
    q.update(res);
}

void sqrtX(Qubit &q) {
    MatrixXcd baseSX = MatrixXd(2, 2);
    baseSX.setZero();
    baseSX(0, 0) = E_TO_IPI_OVER_4;
    baseSX(0, 1) = E_TO_MINUS_IPI_OVER_4;
    baseSX(1, 0) = E_TO_MINUS_IPI_OVER_4;
    baseSX(1, 1) = E_TO_IPI_OVER_4;
    baseSX *= ONE_OVER_SQRT2;
    
    MatrixXcd sx = baseSX;
    
    for (int i = 1; i < q.size(); ++i) {
        sx = kroneckerProduct(baseSX, sx);
        //db(i << " sx:\n" << sx);
    }
    
    VectorXcd v = q.toVector();
    VectorXcd res = (sx * v);
    q.update(res);
}

void sGate(Qubit &q) {
    MatrixXcd baseS = MatrixXd(2, 2);
    baseS.setZero();
    baseS(0, 0) = 1;
    baseS(1, 1) = 1i;
    MatrixXcd s = baseS;
    
    for (int i = 1; i < q.size(); ++i) {
        s = kroneckerProduct(baseS, s);
        //db(i << " s:\n" << s);
    }
    
    VectorXcd v = q.toVector();
    VectorXcd res = (s * v);
    q.update(res);
}

void tGate(Qubit &q) {
    MatrixXcd baseT = MatrixXd(2, 2);
    baseT.setZero();
    baseT(0, 0) = 1;
    baseT(1, 1) = E_TO_IPI_OVER_4;
    MatrixXcd t = baseT;
    
    for (int i = 1; i < q.size(); ++i) {
        t = kroneckerProduct(baseT, t);
        //db(i << " t:\n" << t);
    }
    
    VectorXcd v = q.toVector();
    VectorXcd res = (t * v);
    q.update(res);
}

void cnot(const Qubit &x, Qubit &y) {
    if (x.isOne()) {
        xGate(y);
    }
}

void registerGates(asIScriptEngine *engine) {
    int r;
    r = engine->RegisterGlobalFunction("void hadamard(Qubit &inout)", asFUNCTION(hadamard), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void xGate(Qubit &inout)", asFUNCTION(xGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void yGate(Qubit &inout)", asFUNCTION(yGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void zGate(Qubit &inout)", asFUNCTION(zGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void sqrtX(Qubit &inout)", asFUNCTION(sqrtX), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void sGate(Qubit &inout)", asFUNCTION(sGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void tGate(Qubit &inout)", asFUNCTION(tGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void cnot(const Qubit &in, Qubit &inout)", asFUNCTION(cnot), asCALL_CDECL); assert(r >= 0);
}
