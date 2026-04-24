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
#include <omp.h>

using namespace std;


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
