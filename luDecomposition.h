#ifndef LINALG_LUDECOMPOSITION_H
#define LINALG_LUDECOMPOSITION_H

#include <exception>
#include "matrix.h"
#include "matrixFactory.h"

namespace LUDecomposition {
    template<typename T>
    struct Decomposition {
        Matrix<T> L;
        Matrix<T> U;

        Decomposition(const Matrix<T> &matrix) : L(MatrixFactory::IdentityMatrix<T>(matrix.rows())), U(matrix) {}
    };

    template<typename T>
    Decomposition<T> Decompose(const Matrix<T> &matrix) {
        const size_t nbRows = matrix.rows();
        const size_t nbColumns = matrix.columns();
        if(nbRows != nbColumns) {
            throw std::domain_error("Matrix is not square.");
        }

        Decomposition<T> decomposition(matrix);
        decomposition.L = MatrixFactory::IdentityMatrix<T>(nbRows);

        for(size_t column = 0; column < nbColumns; ++column) {
            for(size_t row = column + 1; row < nbRows; ++row) {
                const T & divisor = decomposition.U(column, column);
                if(std::fabs(divisor) < std::numeric_limits<T>::min()) {
                    throw std::domain_error("Division by 0."); //a_ii != 0 is necessary because of pivoting with diaognal strategy
                }
                
                decomposition.L(row, column) = decomposition.U(row, column) / divisor;

                for(size_t col = column; col < nbColumns; ++col) {
                    decomposition.U(row, col) -= decomposition.L(row, column) * decomposition.U(column, col);
                }
            }
        }

        return decomposition;
    }
}

TEST_SUITE("Matrix solve test suite") {
    TEST_CASE("Matrix Decomposition") {
        static const double EPSILON = 1e-10;
        SUBCASE("LU-Decomposition Test 1") {
            //     |1 2 3|
            // A = |1 1 1|
            //     |3 3 1|

            Matrix<double> A = {
                3, 3, (std::array<double, 9>{1, 2, 3, 1, 1, 1, 3, 3, 1}).data()
            };
            LUDecomposition::Decomposition<double> decomposition = LUDecomposition::Decompose(A);

            Matrix test = decomposition.L * decomposition.U;

            CHECK(TestUtils::CompareMatrix(test, A, false, EPSILON));
        }

        SUBCASE("LU-Decomposition Test 2") {
            //     |2.1  2512 -2516|
            // A = |-1.3  8.8  -7.6|
            //     |0.9   -6.2  4.6|

            Matrix<double> A = {
                3, 3, (std::array<double, 9>{2.1, 2512, -2516, -1.3, 8.8, -7.6, 0.9, -6.2, 4.6}).data()
            };
            LUDecomposition::Decomposition<double> decomposition = LUDecomposition::Decompose(A);

            Matrix test = decomposition.L * decomposition.U;
            CHECK(TestUtils::CompareMatrix(test, A, false, EPSILON));
        }

        SUBCASE("LU-Decomposition Test 3, Error") {
            //     |0 2 3|
            // A = |1 0 1|
            //     |3 3 0|

            Matrix<double> A = {
                3, 3, (std::array<double, 9>{0, 2, 3, 0, 1, 1, 0, 3, 1}).data()
            };
            CHECK_THROWS_AS(LUDecomposition::Decompose(A), std::domain_error);
        }
    }
}


#endif //LINALG_LUDECOMPOSITION_H
