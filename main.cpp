
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <functional>
#include <fstream>
#include <utility>
#include "aux.cpp"


using namespace std;

// ---------------------------------------------------------
// 1. NaivOnArray
// ---------------------------------------------------------
void multiplicarNaivOnArray(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int64_t suma = 0;
            for (int k = 0; k < n; k++) {
                suma += A[i][k] * B[k][j];
            }
            C[i][j] = suma;
        }
    }
}

// ---------------------------------------------------------
// 2. NaivLoopUnrollingTwo
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 3. NaivLoopUnrollingFour
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 4. WinogradOriginal
// ---------------------------------------------------------
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



// ---------------------------------------------------------
// 8. III.3 Sequential block (Row by Column)
// ---------------------------------------------------------

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

// ---------------------------------------------------------
// 9. III.4 Parallel Block (Row by Column)
// ---------------------------------------------------------

void multiplicarBlockPar_III(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
#pragma omp parallel for collapse(2)
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

// ---------------------------------------------------------
// 10. III.5 Enhanced Parallel Block (Row by Column + Unrolling en k)
// ---------------------------------------------------------
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


// ---------------------------------------------------------
// 11. IV.3 Sequential block (Row by Row)
// ---------------------------------------------------------

void multiplicarBlockSeq_IV(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
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

// ---------------------------------------------------------
// 12. IV.4 Parallel Block (Row by Row)
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 13. IV.5 Enhanced Parallel Block (Row by Row + Unrolling en j)
// ---------------------------------------------------------

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


// ---------------------------------------------------------
// 14. V.3 Sequential block (Column by Column)
// ---------------------------------------------------------

void multiplicarBlockSeq_V(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
    for (int j = 0; j < n; j += blockSize) {
        for (int i = 0; i < n; i += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                for (int jj = j; jj < min(j + blockSize, n); jj++) {
                    for (int ii = i; ii < min(i + blockSize, n); ii++) {
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

// ---------------------------------------------------------
// 15. V.4 Parallel Block (Column by Column)
// ---------------------------------------------------------

void multiplicarBlockPar_V(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize) {
    // Paralelizamos SOLO por las columnas (j)
#pragma omp parallel for
    for (int j = 0; j < n; j += blockSize) {
        for (int i = 0; i < n; i += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                for (int jj = j; jj < min(j + blockSize, n); jj++) {
                    for (int ii = i; ii < min(i + blockSize, n); ii++) {
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



// --- MÓDULO DE PERSISTENCIA ---
void guardarResultadosCSV(const string& nombreArchivo, int n, const vector<pair<string, double>>& resultados) {
    // Abrimos el archivo en modo 'app' (append) para no borrar los casos anteriores
    ofstream archivo(nombreArchivo, ios::app);

    if (!archivo.is_open()) {
        cerr << "Error: No se pudo crear el archivo " << nombreArchivo << endl;
        return;
    }

    // Escribimos un encabezado para identificar el caso de prueba
    archivo << "--- CASO DE PRUEBA: Matriz " << n << "x" << n << " ---\n";
    archivo << "Algoritmo,Tiempo (segundos)\n";

    for (const auto& res : resultados) {
        archivo << res.first << "," << res.second << "\n";
    }
    archivo << "\n"; // Espacio para el siguiente caso

    archivo.close();
    cout << ">> Tiempos guardados exitosamente en: " << nombreArchivo << "\n\n";
}

// Función para guardar una matriz en un archivo de texto plano
void guardarMatrizEnArchivo(const string& nombreArchivo, const vector<vector<int64_t>>& matriz) {
    ofstream archivo(nombreArchivo); // Crea o sobrescribe el archivo

    if (archivo.is_open()) {
        int n = matriz.size();

        // Guardamos el tamaño de la matriz en la primera línea (Excelente práctica)
        archivo << n << "\n";

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // Escribimos cada número separado por un espacio
                archivo << matriz[i][j] << " ";
            }
            // Al terminar la fila, damos un salto de línea
            archivo << "\n";
        }

        archivo.close();
        cout << ">> Matriz guardada exitosamente en: " << nombreArchivo << "\n";
    } else {
        cerr << "Error: No se pudo crear o abrir el archivo " << nombreArchivo << "\n";
    }
}


void ejecutarYMedir(string nombreAlgoritmo, function<void()> algoritmo, double& tiempoResultado) {

    // Iniciar medición exclusiva de este hilo
    auto start = chrono::high_resolution_clock::now();

    // Ejecutar el algoritmo
    algoritmo();

    // Detener medición
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duracion = end - start;

    tiempoResultado = duracion.count(); // Guardamos el tiempo en la variable externa
    cout << "[Finalizado] " << nombreAlgoritmo << " tomo: " << tiempoResultado << " segundos.\n";
}




int main() {
    // REQUERIMIENTO 1: Casos de prueba (256 y 512)
    vector<int> casosDePrueba = {2048, 512};
    string nombreArchivoCSV = "resultados_benchmark.csv";
    int blockSize = 32; // Tamaño de bloque ideal para la Caché L1

    // Limpiar el archivo CSV antes de empezar la carrera
    ofstream limpiarArchivo(nombreArchivoCSV);
    limpiarArchivo.close();

    for (int n : casosDePrueba) {
        cout << "\n=================================================\n";
        cout << "INICIANDO CASO DE PRUEBA: Matriz " << n << "x" << n << "\n";
        cout << "=================================================\n";

        // 1. Crear matrices A y B
        vector<vector<int64_t>> A(n, vector<int64_t>(n, 0));
        vector<vector<int64_t>> B(n, vector<int64_t>(n, 0));

        // 2. Inicializarlas con números de 6 dígitos
        inicializarMatriz(A, n);
        inicializarMatriz(B, n);

        // 3. PERSISTENCIA DE MATRICES (Requerimiento 1)
        // Guarda los datos en archivos de texto dinámicos (ej. MatrizA_256.txt)
        guardarMatrizEnArchivo("MatrizA_" + to_string(n) + ".txt", A);
        guardarMatrizEnArchivo("MatrizB_" + to_string(n) + ".txt", B);

        // Imprimir muestra visual en consola
        imprimirMuestraMatriz("Matriz A (Entrada)", A, 5);
        imprimirMuestraMatriz("Matriz B (Entrada)", B, 5);

        // 4. CREAR MATRICES DE RESULTADO INDEPENDIENTES
        vector<vector<int64_t>> C1_Naiv(n, vector<int64_t>(n, 0)), C2_Unroll2(n, vector<int64_t>(n, 0)),
                C3_Unroll4(n, vector<int64_t>(n, 0)), C4_WinOrig(n, vector<int64_t>(n, 0)),
                C5_WinScale(n, vector<int64_t>(n, 0)), C6_Strassen(n, vector<int64_t>(n, 0)),
                C7_StrassenWin(n, vector<int64_t>(n, 0)), C8(n, vector<int64_t>(n, 0)),
                C9(n, vector<int64_t>(n, 0)), C10(n, vector<int64_t>(n, 0)),
                C11(n, vector<int64_t>(n, 0)), C12(n, vector<int64_t>(n, 0)),
                C13(n, vector<int64_t>(n, 0)), C14(n, vector<int64_t>(n, 0)),
                C15(n, vector<int64_t>(n, 0));

        // 5. VARIABLES PARA CAPTURAR LOS TIEMPOS DE CADA HILO
        double t1=0, t2=0, t3=0, t4=0, t5=0, t6=0, t7=0, t8=0, t9=0, t10=0, t11=0, t12=0, t13=0, t14=0, t15=0;

        cout << "Lanzando 15 hilos simultaneos...\n";

        // 6. CREACIÓN DE HILOS (Se pasa la tarea y la variable de tiempo por referencia)
        thread h1(ejecutarYMedir, "1. NaivOnArray",           [&](){ multiplicarNaivOnArray(A, B, C1_Naiv, n); }, std::ref(t1));
        thread h2(ejecutarYMedir, "2. NaivLoopUnrollingTwo",  [&](){ multiplicarNaivLoopUnrollingTwo(A, B, C2_Unroll2, n); }, std::ref(t2));
        thread h3(ejecutarYMedir, "3. NaivLoopUnrollingFour", [&](){ multiplicarNaivLoopUnrollingFour(A, B, C3_Unroll4, n); }, std::ref(t3));
        thread h4(ejecutarYMedir, "4. Winograd Original",     [&](){ multiplicarWinogradOriginal(A, B, C4_WinOrig, n); }, std::ref(t4));
        thread h5(ejecutarYMedir, "5. Winograd Scaled",       [&](){ multiplicarWinogradScaled(A, B, C5_WinScale, n); }, std::ref(t5));
        thread h6(ejecutarYMedir, "6. StrassenNaiv",          [&](){ multiplicarStrassenNaiv(A, B, C6_Strassen, n); }, std::ref(t6));
        thread h7(ejecutarYMedir, "7. Strassen Winograd",     [&](){ multiplicarStrassenWinograd(A, B, C7_StrassenWin, n); }, std::ref(t7));
        thread h8(ejecutarYMedir, "8. III.3 Block Sequential", [&](){ multiplicarBlockSeq_III(A, B, C8, n, blockSize); }, std::ref(t8));
        thread h9(ejecutarYMedir, "9. III.4 Block Parallel",   [&](){ multiplicarBlockPar_III(A, B, C9, n, blockSize); }, std::ref(t9));
        thread h10(ejecutarYMedir,"10. III.5 Block Enhanced",  [&](){ multiplicarBlockEnh_III(A, B, C10, n, blockSize); }, std::ref(t10));
        thread h11(ejecutarYMedir,"11. IV.3 Block Sequential", [&](){ multiplicarBlockSeq_IV(A, B, C11, n, blockSize); }, std::ref(t11));
        thread h12(ejecutarYMedir,"12. IV.4 Block Parallel",   [&](){ multiplicarBlockPar_IV(A, B, C12, n, blockSize); }, std::ref(t12));
        thread h13(ejecutarYMedir,"13. IV.5 Block Enhanced",   [&](){ multiplicarBlockEnh_IV(A, B, C13, n, blockSize); }, std::ref(t13));
        thread h14(ejecutarYMedir,"14. V.3 Block Sequential",  [&](){ multiplicarBlockSeq_V(A, B, C14, n, blockSize); }, std::ref(t14));
        thread h15(ejecutarYMedir,"15. V.4 Block Parallel",    [&](){ multiplicarBlockPar_V(A, B, C15, n, blockSize); }, std::ref(t15));

        // 7. SINCRONIZACIÓN
        h1.join();  h2.join();  h3.join();  h4.join();  h5.join();
        h6.join();  h7.join();  h8.join();  h9.join();  h10.join();
        h11.join(); h12.join(); h13.join(); h14.join(); h15.join();

        cout << "\nTodos los algoritmos terminaron para n=" << n << ".\n";

        // 8. RECOPILAR Y GUARDAR EN CSV
        vector<pair<string, double>> resultados = {
                {"1. NaivOnArray", t1}, {"2. NaivLoopUnrollingTwo", t2}, {"3. NaivLoopUnrollingFour", t3},
                {"4. Winograd Original", t4}, {"5. Winograd Scaled", t5}, {"6. StrassenNaiv", t6},
                {"7. Strassen Winograd", t7}, {"8. III.3 Block Sequential", t8}, {"9. III.4 Block Parallel", t9},
                {"10. III.5 Block Enhanced", t10}, {"11. IV.3 Block Sequential", t11}, {"12. IV.4 Block Parallel", t12},
                {"13. IV.5 Block Enhanced", t13}, {"14. V.3 Block Sequential", t14}, {"15. V.4 Block Parallel", t15}
        };

        guardarResultadosCSV(nombreArchivoCSV, n, resultados);
    }

    cout << "\n=================================================\n";
    cout << "PROYECTO FINALIZADO CON ÉXITO.\n";
    cout << "=================================================\n";

    return 0;
}
