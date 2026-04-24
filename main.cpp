
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
#include "functions.h"

using namespace std;


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
