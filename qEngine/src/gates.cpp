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

double degToRad(double val) {
    return 0.01745329251994329576923690768489 * val;
}

double radToDeg(double val) {
    return 57.295779513082320876798154814105 * val;
}

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

void rxGate(Qubit &q, double theta) {
    double rad2 = degToRad(theta) / 2;
    
    MatrixXcd baseRX = MatrixXd(2, 2);
    baseRX(0, 0) = std::cos(rad2);
    baseRX(0, 1) = -1i * std::sin(rad2);
    baseRX(1, 0) = -1i * std::sin(rad2);
    baseRX(1, 1) = std::cos(rad2);
    MatrixXcd rx = baseRX;
    
    for (int i = 1; i < q.size(); ++i) {
        rx = kroneckerProduct(baseRX, rx);
        //db(i << " rx:\n" << rx);
    }
    
    VectorXcd v = q.toVector();
    VectorXcd res = (rx * v);
    q.update(res);
}

void ryGate(Qubit &q, double theta) {
    double rad2 = degToRad(theta) / 2;
    
    MatrixXcd baseRY = MatrixXd(2, 2);
    baseRY(0, 0) = std::cos(rad2);
    baseRY(0, 1) = -std::sin(rad2);
    baseRY(1, 0) = std::sin(rad2);
    baseRY(1, 1) = std::cos(rad2);
    MatrixXcd ry = baseRY;
    
    for (int i = 1; i < q.size(); ++i) {
        ry = kroneckerProduct(baseRY, ry);
        //db(i << " ry:\n" << ry);
    }
    
    VectorXcd v = q.toVector();
    VectorXcd res = (ry * v);
    q.update(res);
}

void rzGate(Qubit &q, double theta) {
    double rad2 = degToRad(theta) / 2;
    
    MatrixXcd baseRY = MatrixXd(2, 2);
    baseRY.setZero();
    baseRY(0, 0) = std::exp(-1i * rad2);
    baseRY(1, 1) = std::exp(1i * rad2);
    MatrixXcd ry = baseRY;
    
    for (int i = 1; i < q.size(); ++i) {
        ry = kroneckerProduct(baseRY, ry);
        //db(i << " ry:\n" << ry);
    }
    
    VectorXcd v = q.toVector();
    VectorXcd res = (ry * v);
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

void swap(Qubit &x, Qubit &y) {
    Component tmp = x.component(1);
    x.setComponent(1, y.component(0));
    y.setComponent(1, tmp);
}

void sqrtSwap(Qubit &x, Qubit &y) {
    MatrixXcd matrix = MatrixXd(4, 4);
    matrix.setZero();
    matrix(0, 0) = 1;
    matrix(1, 1) = Component(0.5, 0.5);
    matrix(1, 2) = Component(0.5, -0.5);
    matrix(2, 1) = Component(0.5, -0.5);
    matrix(2, 2) = Component(0.5, 0.5);
    matrix(3, 3) = 1;
    
    Qubit tmp(Qubit::Zero, 2);
    tmp.setComponent(0, x.component(0));
    tmp.setComponent(1, x.component(1));
    tmp.setComponent(2, y.component(0));
    tmp.setComponent(3, y.component(1));
    
    VectorXcd v = tmp.toVector();
    VectorXcd res = (matrix * v);
    tmp.update(res);
    db(tmp.isValid());
    
    x.setComponent(0, tmp.component(0));
    x.setComponent(1, tmp.component(1));
    y.setComponent(0, tmp.component(2));
    y.setComponent(1, tmp.component(3));
}

void registerGates(asIScriptEngine *engine) {
    int r;
    r = engine->RegisterGlobalFunction("void hadamard(Qubit &inout)", asFUNCTION(hadamard), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void xGate(Qubit &inout)", asFUNCTION(xGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void yGate(Qubit &inout)", asFUNCTION(yGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void zGate(Qubit &inout)", asFUNCTION(zGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void rxGate(Qubit &inout, double)", asFUNCTION(rxGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void ryGate(Qubit &inout, double)", asFUNCTION(ryGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void rzGate(Qubit &inout, double)", asFUNCTION(rzGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void sqrtX(Qubit &inout)", asFUNCTION(sqrtX), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void sGate(Qubit &inout)", asFUNCTION(sGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void tGate(Qubit &inout)", asFUNCTION(tGate), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void cnot(const Qubit &in, Qubit &inout)", asFUNCTION(cnot), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void swap(Qubit &inout, Qubit &inout)", asFUNCTION(swap), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void sqrtSwap(Qubit &inout, Qubit &inout)", asFUNCTION(sqrtSwap), asCALL_CDECL); assert(r >= 0);
}
