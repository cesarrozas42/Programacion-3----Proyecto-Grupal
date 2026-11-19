cmake_minimum_required(VERSION 3.20)
project(plataforma_streaming LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------
# Carpetas del proyecto: una por integrante + la base de la parte 1
# ------------------------------------------------------------------
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante1_Arbol
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante2_Paralelismo
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante3_Singleton_Factory
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante4_Strategy_Observer
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante5_Interfaz)

# ------------------------------------------------------------------
# Nucleo compartido por todos los ejecutables
# ------------------------------------------------------------------
set(FUENTES_NUCLEO
    Integrante1_Arbol/Trie.cpp               # Integrante 1 (Parte 2): trie de palabras + trie de sufijos
    Integrante1_Arbol/Indexador.cpp          # Integrante 1 (Parte 2): un arbol por tipo de tag
    Integrante3_Singleton_Factory/CatalogoPeliculas.cpp  # Integrante 3 (Parte 2): Singleton
    Integrante3_Singleton_Factory/PeliculaFactory.cpp    # Integrante 3 (Parte 2): Factory
    Integrante4_Strategy_Observer/EstrategiaBusqueda.cpp # Integrante 4 (Parte 2): Strategy (arbol / secuencial / paralela)
    CSVReader.cpp
    Motor_de_busqueda.cpp
    Pelicula.cpp
    Recomendador.cpp
    Relevancia.cpp
    Tag.cpp
    Usuario.cpp)

add_library(streaming_core ${FUENTES_NUCLEO})

# Hilos: estrategia paralela (std::async) y mutex del Singleton
find_package(Threads REQUIRED)
target_link_libraries(streaming_core PUBLIC Threads::Threads)

if(MSVC)
    target_compile_options(streaming_core PRIVATE /W4)
else()
    target_compile_options(streaming_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------
# Programa principal: menu de CineSearch (Integrante 5 - interfaz)
# En CLion: elegir "plataforma_streaming" y darle Run.
# ------------------------------------------------------------------
add_executable(plataforma_streaming Integrante5_Interfaz/main.cpp Integrante5_Interfaz/InterfazCLI.cpp)
target_link_libraries(plataforma_streaming PRIVATE streaming_core)

# ------------------------------------------------------------------
# Pruebas (ctest --test-dir build --output-on-failure)
# ------------------------------------------------------------------
enable_testing()

add_executable(tests_proyecto test_relevancia_recomendador.cpp test_motor.cpp)
target_link_libraries(tests_proyecto PRIVATE streaming_core)
add_test(NAME pruebas_del_proyecto COMMAND tests_proyecto)

# Arbol (Integrante 1 - Parte 2): pruebas unitarias y demo para la exposicion
add_executable(tests_arbol Integrante1_Arbol/test_arbol.cpp)
target_link_libraries(tests_arbol PRIVATE streaming_core)
add_test(NAME pruebas_del_arbol COMMAND tests_arbol)

add_executable(demo_arbol Integrante1_Arbol/demo_arbol.cpp)
target_link_libraries(demo_arbol PRIVATE streaming_core)

# Patrones A (Integrante 3 - Parte 2): Singleton + Factory
add_executable(test_patrones Integrante3_Singleton_Factory/test_patrones.cpp)
target_link_libraries(test_patrones PRIVATE streaming_core)
add_test(NAME pruebas_de_patrones COMMAND test_patrones)

# ------------------------------------------------------------------
# Falta: Integrante 2 (carga paralela del CSV). Sumar sus .cpp a
# FUENTES_NUCLEO; los hilos ya estan enlazados arriba y el catalogo
# --- AGREGAMOS EL BENCHMARK AQUI ---
add_executable(benchmark
        Integrante2_Paralelismo/benchmark.cpp
        CSVReader.cpp
        Integrante1_Arbol/Indexador.cpp
        Pelicula.cpp
        Tag.cpp
        Integrante1_Arbol/Trie.cpp
        Integrante3_Singleton_Factory/PeliculaFactory.cpp
        Integrante2_Paralelismo/ParallelEngine.cpp)
target_link_libraries(benchmark Threads::Threads)
# ------------------------------------------------------------------
