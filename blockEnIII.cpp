//
// Created by Nicolas on 24/04/26.
//


#include <vector>
#include "functions.h"
#include <omp.h>

void multiplicarBlockEnh_III(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i += blockSize) {
        for (int j = 0; j < n; j += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                for (int ii = i; ii < min(i + blockSize, n); ii++) {
                    for (int jj = j; jj < min(j + blockSize, n); jj++) {
                        int64_t suma = 0;
                        int limite_k = min(k + blockSize, n);
                        int kk = k;

                        // Loop Unrolling de 4 en 4
                        for (; kk <= limite_k - 4; kk += 4) {
                            suma += A[ii][kk]   * B[kk][jj]   +
                                    A[ii][kk+1] * B[kk+1][jj] +
                                    A[ii][kk+2] * B[kk+2][jj] +
                                    A[ii][kk+3] * B[kk+3][jj];
                        }
                        for (; kk < limite_k; kk++) {
                            suma += A[ii][kk] * B[kk][jj];
                        }
                        C[ii][jj] += suma;
                    }
                }
            }
        }
    }
}
