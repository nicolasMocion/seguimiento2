//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"

using namespace std;

// ---------------------------------------------------------
// 6: Strassen-Naiv
// ---------------------------------------------------------

void multiplicarStrassenNaiv(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {

    // CASO BASE: Si la matriz es pequeña, es más rápido usar el método clásico (Naiv)

    if (n <= 64) {
        multiplicarNaivOnArray(A, B, C, n);
        return;
    }

    int mitad = n / 2;

    // 1. Inicialización de las sub-matrices (Cuadrantes)
    vector<vector<int64_t>> A11(mitad, vector<int64_t>(mitad)), A12(mitad, vector<int64_t>(mitad)),
            A21(mitad, vector<int64_t>(mitad)), A22(mitad, vector<int64_t>(mitad));
    vector<vector<int64_t>> B11(mitad, vector<int64_t>(mitad)), B12(mitad, vector<int64_t>(mitad)),
            B21(mitad, vector<int64_t>(mitad)), B22(mitad, vector<int64_t>(mitad));

    // 2. Extraer los datos de las matrices gigantes A y B hacia los cuadrantes
    for (int i = 0; i < mitad; i++) {
        for (int j = 0; j < mitad; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + mitad];
            A21[i][j] = A[i + mitad][j];
            A22[i][j] = A[i + mitad][j + mitad];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + mitad];
            B21[i][j] = B[i + mitad][j];
            B22[i][j] = B[i + mitad][j + mitad];
        }
    }

    // Matrices temporales para guardar resultados de sumas/restas
    vector<vector<int64_t>> tempA(mitad, vector<int64_t>(mitad));
    vector<vector<int64_t>> tempB(mitad, vector<int64_t>(mitad));

    // Las 7 multiplicaciones mágicas de Strassen (M1 a M7)
    vector<vector<int64_t>> M1(mitad, vector<int64_t>(mitad)), M2(mitad, vector<int64_t>(mitad)),
            M3(mitad, vector<int64_t>(mitad)), M4(mitad, vector<int64_t>(mitad)),
            M5(mitad, vector<int64_t>(mitad)), M6(mitad, vector<int64_t>(mitad)),
            M7(mitad, vector<int64_t>(mitad));

    // M1 = (A11 + A22) * (B11 + B22)
    sumarMatrices(A11, A22, tempA, mitad);
    sumarMatrices(B11, B22, tempB, mitad);
    multiplicarStrassenNaiv(tempA, tempB, M1, mitad); // LLAMADA RECURSIVA

    // M2 = (A21 + A22) * B11
    sumarMatrices(A21, A22, tempA, mitad);
    multiplicarStrassenNaiv(tempA, B11, M2, mitad);

    // M3 = A11 * (B12 - B22)
    restarMatrices(B12, B22, tempB, mitad);
    multiplicarStrassenNaiv(A11, tempB, M3, mitad);

    // M4 = A22 * (B21 - B11)
    restarMatrices(B21, B11, tempB, mitad);
    multiplicarStrassenNaiv(A22, tempB, M4, mitad);

    // M5 = (A11 + A12) * B22
    sumarMatrices(A11, A12, tempA, mitad);
    multiplicarStrassenNaiv(tempA, B22, M5, mitad);

    // M6 = (A21 - A11) * (B11 + B12)
    restarMatrices(A21, A11, tempA, mitad);
    sumarMatrices(B11, B12, tempB, mitad);
    multiplicarStrassenNaiv(tempA, tempB, M6, mitad);

    // M7 = (A12 - A22) * (B21 + B22)
    restarMatrices(A12, A22, tempA, mitad);
    sumarMatrices(B21, B22, tempB, mitad);
    multiplicarStrassenNaiv(tempA, tempB, M7, mitad);

    // 4. Calcular los cuadrantes de la matriz final C (C11, C12, C21, C22)
    for (int i = 0; i < mitad; i++) {
        for (int j = 0; j < mitad; j++) {
            // C11 = M1 + M4 - M5 + M7
            C[i][j] = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j];

            // C12 = M3 + M5
            C[i][j + mitad] = M3[i][j] + M5[i][j];

            // C21 = M2 + M4
            C[i + mitad][j] = M2[i][j] + M4[i][j];

            // C22 = M1 - M2 + M3 + M6
            C[i + mitad][j + mitad] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j];
        }
    }
}
