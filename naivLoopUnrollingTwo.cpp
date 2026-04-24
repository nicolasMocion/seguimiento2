
#include <vector>
#include "functions.h"

using namespace std;

void multiplicarNaivLoopUnrollingTwo(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int64_t suma = 0;
            // El ciclo avanza de 2 en 2
            for (int k = 0; k < n; k += 2) {
                // Sumamos el elemento actual y el siguiente en la misma línea
                suma += A[i][k] * B[k][j] +
                        A[i][k + 1] * B[k + 1][j];
            }
            C[i][j] = suma;
        }
    }
}