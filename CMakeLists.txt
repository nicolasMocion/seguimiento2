cmake_minimum_required(VERSION 3.27)
project(seguimiento2)

set(CMAKE_CXX_STANDARD 20)

# --- CONFIGURACIÓN ESTRICTA PARA MAC (APPLE SILICON) ---
set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")

set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_C_LIB_NAMES "omp") # <--- Esta es la línea clave que le faltaba a CMake

set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)

# Buscar OpenMP
find_package(OpenMP REQUIRED)

# --- CREACIÓN DEL EJECUTABLE ---
add_executable(seguimiento2 main.cpp
        aux.cpp
        NaivOnArray.cpp
        naivLoopUnrollingTwo.cpp
        naivLoopUnrollingFour.cpp
        Winograd.cpp
        functions.h
        winogradScaled.cpp
        strassenNaiv.cpp
        strassenWin.cpp
        bloqSeqIII.cpp
        parallelbloqIII.cpp
        blockEnIII.cpp
        blockSeqIV.cpp
        blockParIV.cpp
        blockEnhIV.cpp
        blockSeqV.cpp
        parallelBlockV.cpp)

# --- VINCULACIÓN ---
if(OpenMP_CXX_FOUND)
    target_link_libraries(seguimiento2 PUBLIC OpenMP::OpenMP_CXX)
endif()