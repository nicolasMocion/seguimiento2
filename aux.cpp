//
// Created by Nicolas osorio on 23/04/26.
//

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <utility>


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
