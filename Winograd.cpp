//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"

using namespace std;

void multiplicarWinogradOriginal(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {

    vector<int64_t> rowFactor(n, 0);
    vector<int64_t> colFactor(n, 0);

    // 1. Precalcular los Factores de Fila (Row Factors) para A
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n / 2; j++) {
            rowFactor[i] += A[i][2 * j] * A[i][2 * j + 1];
        }
    }

    // 2. Precalcular los Factores de Columna (Column Factors) para B
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n / 2; j++) {
            colFactor[i] += B[2 * j][i] * B[2 * j + 1][i];
        }
    }

    // 3. Calcular la matriz C usando los factores precalculados
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Inicializamos la suma restando los factores
            int64_t suma = -rowFactor[i] - colFactor[j];

            // Avanzamos de 1 en 1, pero cubrimos 2 elementos por el (n/2)
            for (int k = 0; k < n / 2; k++) {
                // Aquí está la magia: solo 1 multiplicación en lugar de 2 por cada par de elementos
                suma += (A[i][2 * k] + B[2 * k + 1][j]) * (A[i][2 * k + 1] + B[2 * k][j]);
            }
            C[i][j] = suma;
        }
    }
}
