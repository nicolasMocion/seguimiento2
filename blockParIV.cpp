//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"
#include <omp.h>

void multiplicarBlockPar_IV(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
    // Paralelizamos SOLO por las filas (i) para asegurar exclusión mutua en la matriz C
#pragma omp parallel for
    for (int i = 0; i < n; i += blockSize) {
        for (int k = 0; k < n; k += blockSize) {
            for (int j = 0; j < n; j += blockSize) {
                for (int ii = i; ii < min(i + blockSize, n); ii++) {
                    for (int kk = k; kk < min(k + blockSize, n); kk++) {
                        int64_t r = A[ii][kk];
                        for (int jj = j; jj < min(j + blockSize, n); jj++) {
                            C[ii][jj] += r * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
}