//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"

using namespace std;

// ---------------------------------------------------------
// 5. WinogradScaled
// ---------------------------------------------------------
void multiplicarWinogradScaled(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {

    // Creamos copias locales para escalarlas
    vector<vector<int64_t>> copyA = A;
    vector<vector<int64_t>> copyB = B;

    // Factor de escala simulado (ej. multiplicar A por 2 y dividir B por 2)
    // Esto balancea los bits sin afectar el resultado final matemático (A*2 * B/2 = A*B)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            copyA[i][j] = copyA[i][j] << 1; // Equivalente ultrarrápido a multiplicar por 2
            copyB[i][j] = copyB[i][j] >> 1; // Equivalente ultrarrápido a dividir por 2
        }
    }

    vector<int64_t> rowFactor(n, 0);
    vector<int64_t> colFactor(n, 0);

    // 1. Row Factors (sobre copyA)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n / 2; j++) {
            rowFactor[i] += copyA[i][2 * j] * copyA[i][2 * j + 1];
        }
    }

    // 2. Col Factors (sobre copyB)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n / 2; j++) {
            colFactor[i] += copyB[2 * j][i] * copyB[2 * j + 1][i];
        }
    }

    // 3. Multiplicación usando la ecuación de Winograd
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int64_t suma = -rowFactor[i] - colFactor[j];
            for (int k = 0; k < n / 2; k++) {
                suma += (copyA[i][2 * k] + copyB[2 * k + 1][j]) * (copyA[i][2 * k + 1] + copyB[2 * k][j]);
            }
            C[i][j] = suma;
        }
    }
}
