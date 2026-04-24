//
// Created by Nicolas on 24/04/26.
//

#ifndef SEGUIMIENTO2_FUNCTIONS_H
#define SEGUIMIENTO2_FUNCTIONS_H

#include <vector>
#include <string>
#include <functional>

using namespace std;

// Firmas de Auxiliares (aux.cpp)
void inicializarMatriz(vector<vector<int64_t>>& matriz, int n);
void imprimirMuestraMatriz(const string& nombre, const vector<vector<int64_t>>& matriz, int limite = 5);
void guardarMatrizEnArchivo(const string& nombreArchivo, const vector<vector<int64_t>>& matriz);
void guardarResultadosCSV(const string& nombreArchivo, int n, const vector<pair<string, double>>& resultados);
void ejecutarYMedir(string nombreAlgoritmo, function<void()> algoritmo, double& tiempoResultado);
void sumarMatrices(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void restarMatrices(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);

// Firmas de Algoritmos (Cada uno en su .cpp)
void multiplicarNaivOnArray(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarStrassenNaiv(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarBlockSeq_IV(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarStrassenWinograd(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarNaivLoopUnrollingFour(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarNaivLoopUnrollingTwo(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarWinogradOriginal(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarWinogradScaled(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n);
void multiplicarBlockSeq_III(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarBlockPar_III(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarBlockEnh_III(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarBlockPar_IV(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarBlockEnh_IV(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarBlockSeq_V(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);
void multiplicarBlockPar_V(const vector<vector<int64_t>>& A, const vector<vector<int64_t>>& B, vector<vector<int64_t>>& C, int n, int blockSize);


#endif //SEGUIMIENTO2_FUNCTIONS_H
