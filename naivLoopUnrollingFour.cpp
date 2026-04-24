//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"

using namespace std;

void multiplicarNaivLoopUnrollingFour(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int64_t suma = 0;
            // El ciclo avanza de 4 en 4
            for (int k = 0; k < n; k += 4) {
                // Sumamos 4 elementos consecutivos
                suma += A[i][k] * B[k][j] +
                        A[i][k + 1] * B[k + 1][j] +
                        A[i][k + 2] * B[k + 2][j] +
                        A[i][k + 3] * B[k + 3][j];
            }
            C[i][j] = suma;
        }
    }
}