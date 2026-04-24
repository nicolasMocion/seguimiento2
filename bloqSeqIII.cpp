//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"

using namespace std;

void multiplicarBlockSeq_III(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
    for (int i = 0; i < n; i += blockSize) {
        for (int j = 0; j < n; j += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                for (int ii = i; ii < min(i + blockSize, n); ii++) {
                    for (int jj = j; jj < min(j + blockSize, n); jj++) {
                        int64_t suma = 0;
                        for (int kk = k; kk < min(k + blockSize, n); kk++) {
                            suma += A[ii][kk] * B[kk][jj];
                        }
                        C[ii][jj] += suma;
                    }
                }
            }
        }
    }
}