
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

void inicializarMatriz(vector<vector<int64_t>>& matriz, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Genera números de mínimo 6 dígitos (ej. 100000 a 999999)
            matriz[i][j] = rand() % 900000 + 100000;
        }
    }
}

// Función para imprimir una muestra de la matriz
void imprimirMuestraMatriz(const string& nombre, const vector<vector<int64_t>>& matriz, int limite = 5) {
    int n = matriz.size();

    // Nos aseguramos de no intentar imprimir más de lo que la matriz tiene
    int filasAMostrar = min(n, limite);
    int columnasAMostrar = min(n, limite);

    cout << "\n=== Muestra de la matriz: " << nombre << " (Primeros " << filasAMostrar << "x" << columnasAMostrar << ") ===" << endl;

    for (int i = 0; i < filasAMostrar; i++) {
        for (int j = 0; j < columnasAMostrar; j++) {
            // setw(8) asegura que todos los números se alineen en columnas de 8 espacios
            cout << setw(8) << matriz[i][j] << " ";
        }
        cout << endl; // Salto de línea al terminar la fila
    }
    cout << "========================================================\n" << endl;
}

// --- Función Auxiliar para Sumar Matrices ---
void sumarMatrices(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

// --- Función Auxiliar para Restar Matrices ---
void restarMatrices(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
}

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


void ejecutarYMedir(string nombreAlgoritmo,
                    void (*funcionMultiplicar)(const vector<vector<int64_t>>&, const vector<vector<int64_t>>&, vector<vector<int64_t>>&, int),
                    const vector<vector<int64_t>>& A,
                    const vector<vector<int64_t>>& B,
                    vector<vector<int64_t>>& C,
                    int n) {

    // Iniciar medición exclusiva de este hilo
    auto start = chrono::high_resolution_clock::now();

    // Ejecutar el algoritmo
    funcionMultiplicar(A, B, C, n);

    // Detener medición
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duracion = end - start;

    // Imprimir el resultado (CUIDADO: std::cout no es thread-safe por defecto,
    // los textos podrían mezclarse en consola, pero para probar sirve).
    cout << "[Finalizado] " << nombreAlgoritmo << " tomo: " << duracion.count() << " segundos.\n";
}



int main() {
    int n = 1024; // Tamaño de la matriz

    // 1. Crear matrices A y B llenas de ceros inicialmente
    vector<vector<int64_t>> A(n, vector<int64_t>(n, 0));
    vector<vector<int64_t>> B(n, vector<int64_t>(n, 0));

    // 2. Llamamos a la función para llenarlas con números aleatorios de 6 dígitos
    inicializarMatriz(A, n);
    inicializarMatriz(B, n);

    // 3. Imprimime una muestra de 5x5 para verificar los datos
    imprimirMuestraMatriz("Matriz A (Entrada)", A, 5);
    imprimirMuestraMatriz("Matriz B (Entrada)", B, 5);

    // 4. CREAR UNA MATRIZ DE RESULTADO INDEPENDIENTE PARA CADA HILO
    vector<vector<int64_t>> C_Naiv1(n, vector<int64_t>(n, 0));
    vector<vector<int64_t>> C_Naiv2(n, vector<int64_t>(n, 0));
    vector<vector<int64_t>> C_Naiv4(n, vector<int64_t>(n, 0));
    vector<vector<int64_t>> C_WinOrig(n, vector<int64_t>(n, 0));
    vector<vector<int64_t>> C_WinScale(n, vector<int64_t>(n, 0));
    vector<vector<int64_t>> C_Strassen(n, vector<int64_t>(n, 0));

    cout << "Lanzando hilos simultaneos...\n";

    // 5. CREACIÓN DE HILOS SIMULTÁNEOS (Cada uno con su propia matriz C)
    thread hilo1(ejecutarYMedir, "1. NaivOnArray", multiplicarNaivOnArray, std::ref(A), std::ref(B), std::ref(C_Naiv1), n);
    thread hilo2(ejecutarYMedir, "2. NaivLoopUnrollingTwo", multiplicarNaivLoopUnrollingTwo, std::ref(A), std::ref(B), std::ref(C_Naiv2), n);
    thread hilo3(ejecutarYMedir, "3. NaivLoopUnrollingFour", multiplicarNaivLoopUnrollingFour, std::ref(A), std::ref(B), std::ref(C_Naiv4), n);
    thread hilo4(ejecutarYMedir, "4. Winograd Original", multiplicarWinogradOriginal, std::ref(A), std::ref(B), std::ref(C_WinOrig), n);
    thread hilo5(ejecutarYMedir, "5. Winograd Scaled", multiplicarWinogradScaled, std::ref(A), std::ref(B), std::ref(C_WinScale), n);
    thread hilo6(ejecutarYMedir, "6. StrassenNaiv", multiplicarStrassenNaiv, std::ref(A), std::ref(B), std::ref(C_Strassen), n);

    // 6. SINCRONIZACIÓN (Esperar a que todos terminen)
    hilo1.join();
    hilo2.join();
    hilo3.join();
    hilo4.join();
    hilo5.join();
    hilo6.join();

    cout << "\nTodos los algoritmos han terminado sus cálculos.\n";

    imprimirMuestraMatriz("Resultado StrassenNaiv", C_Strassen, 4);

    return 0;
}



