//
// Created by Nicolas on 24/04/26.
//

#include <vector>
#include "functions.h"

using namespace std;


// ---------------------------------------------------------
// 7. Strassen-Winograd
// ---------------------------------------------------------
void multiplicarStrassenWinograd(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {

    // CASO BASE: Frenar la recursión para matrices pequeñas
    if (n <= 64) {
        multiplicarNaivOnArray(A, B, C, n);
        return;
    }

    int mitad = n / 2;

    // 1. Extraer los cuadrantes (Igual que en Strassen Clásico)
    vector<vector<int64_t>> A11(mitad, vector<int64_t>(mitad)), A12(mitad, vector<int64_t>(mitad)),
            A21(mitad, vector<int64_t>(mitad)), A22(mitad, vector<int64_t>(mitad));
    vector<vector<int64_t>> B11(mitad, vector<int64_t>(mitad)), B12(mitad, vector<int64_t>(mitad)),
            B21(mitad, vector<int64_t>(mitad)), B22(mitad, vector<int64_t>(mitad));

    for (int i = 0; i < mitad; i++) {
        for (int j = 0; j < mitad; j++) {
            A11[i][j] = A[i][j];               A12[i][j] = A[i][j + mitad];
            A21[i][j] = A[i + mitad][j];       A22[i][j] = A[i + mitad][j + mitad];
            B11[i][j] = B[i][j];               B12[i][j] = B[i][j + mitad];
            B21[i][j] = B[i + mitad][j];       B22[i][j] = B[i + mitad][j + mitad];
        }
    }

    // Declaración de variables temporales de Winograd
    vector<vector<int64_t>> S1(mitad, vector<int64_t>(mitad)), S2(mitad, vector<int64_t>(mitad)),
            S3(mitad, vector<int64_t>(mitad)), S4(mitad, vector<int64_t>(mitad)),
            S5(mitad, vector<int64_t>(mitad)), S6(mitad, vector<int64_t>(mitad)),
            S7(mitad, vector<int64_t>(mitad)), S8(mitad, vector<int64_t>(mitad));

    vector<vector<int64_t>> M1(mitad, vector<int64_t>(mitad)), M2(mitad, vector<int64_t>(mitad)),
            M3(mitad, vector<int64_t>(mitad)), M4(mitad, vector<int64_t>(mitad)),
            M5(mitad, vector<int64_t>(mitad)), M6(mitad, vector<int64_t>(mitad)),
            M7(mitad, vector<int64_t>(mitad));

    vector<vector<int64_t>> V1(mitad, vector<int64_t>(mitad)), V2(mitad, vector<int64_t>(mitad));

    // --- 2. Las 8 sumas/restas de pre-cálculo (Ecuaciones de Winograd) ---
    sumarMatrices(A21, A22, S1, mitad);    // S1 = A21 + A22
    restarMatrices(S1, A11, S2, mitad);    // S2 = S1 - A11
    restarMatrices(A11, A21, S3, mitad);   // S3 = A11 - A21
    restarMatrices(A12, S2, S4, mitad);    // S4 = A12 - S2
    restarMatrices(B12, B11, S5, mitad);   // S5 = B12 - B11
    restarMatrices(B22, S5, S6, mitad);    // S6 = B22 - S5
    restarMatrices(B22, B12, S7, mitad);   // S7 = B22 - B12
    restarMatrices(S6, B21, S8, mitad);    // S8 = S6 - B21

    // --- 3. Las 7 Multiplicaciones Recursivas ---
    multiplicarStrassenWinograd(S2, S6, M1, mitad);  // M1 = S2 * S6
    multiplicarStrassenWinograd(A11, B11, M2, mitad); // M2 = A11 * B11
    multiplicarStrassenWinograd(A12, B21, M3, mitad); // M3 = A12 * B21
    multiplicarStrassenWinograd(S3, S7, M4, mitad);  // M4 = S3 * S7
    multiplicarStrassenWinograd(S1, S5, M5, mitad);  // M5 = S1 * S5
    multiplicarStrassenWinograd(S4, B22, M6, mitad); // M6 = S4 * B22
    multiplicarStrassenWinograd(A22, S8, M7, mitad); // M7 = A22 * S8

    // --- 4. Las 2 sumas intermedias ---
    sumarMatrices(M1, M2, V1, mitad);      // V1 = M1 + M2
    sumarMatrices(V1, M4, V2, mitad);      // V2 = V1 + M4

    // --- 5. Ensamblaje final de la matriz C (Las últimas 5 sumas/restas) ---
    for (int i = 0; i < mitad; i++) {
        for (int j = 0; j < mitad; j++) {
            // C11 = M2 + M3
            C[i][j] = M2[i][j] + M3[i][j];

            // C12 = V1 + M5 + M6
            C[i][j + mitad] = V1[i][j] + M5[i][j] + M6[i][j];

            // C21 = V2 - M7
            C[i + mitad][j] = V2[i][j] - M7[i][j];

            // C22 = V2 + M5
            C[i + mitad][j + mitad] = V2[i][j] + M5[i][j];
        }
    }
}
