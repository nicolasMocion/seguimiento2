//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"
#include <omp.h>

void multiplicarBlockEnh_IV(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
#pragma omp parallel for
    for (int i = 0; i < n; i += blockSize) {
        for (int k = 0; k < n; k += blockSize) {
            for (int j = 0; j < n; j += blockSize) {
                for (int ii = i; ii < min(i + blockSize, n); ii++) {
                    for (int kk = k; kk < min(k + blockSize, n); kk++) {
                        int64_t r = A[ii][kk];
                        int limite_j = min(j + blockSize, n);
                        int jj = j;

                        // Como jj es el ciclo interno aquí, el unrolling se hace sobre jj
                        for (; jj <= limite_j - 4; jj += 4) {
                            C[ii][jj]   += r * B[kk][jj];
                            C[ii][jj+1] += r * B[kk][jj+1];
                            C[ii][jj+2] += r * B[kk][jj+2];
                            C[ii][jj+3] += r * B[kk][jj+3];
                        }
                        for (; jj < limite_j; jj++) {
                            C[ii][jj] += r * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
}

