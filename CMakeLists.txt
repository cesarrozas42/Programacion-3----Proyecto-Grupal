cmake_minimum_required(VERSION 3.20)
project(plataforma_streaming LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------
# Nucleo compartido por todos los ejecutables
# ------------------------------------------------------------------
set(FUENTES_NUCLEO
    CatalogoPeliculas.cpp  # Integrante 3 (Parte 2): Singleton
    CSVReader.cpp
    EstrategiaBusqueda.cpp # Integrante 4 (Parte 2): Strategy (arbol / secuencial / paralela)
    Indexador.cpp          # Integrante 1 (Parte 2): un arbol por tipo de tag
    Motor_de_busqueda.cpp
    Pelicula.cpp
    PeliculaFactory.cpp    # Integrante 3 (Parte 2): Factory
    Recomendador.cpp
    Relevancia.cpp
    Tag.cpp
    Trie.cpp               # Integrante 1 (Parte 2): trie de palabras + trie de sufijos
    Usuario.cpp)

add_library(streaming_core ${FUENTES_NUCLEO})
target_include_directories(streaming_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

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
add_executable(plataforma_streaming main.cpp InterfazCLI.cpp)
target_link_libraries(plataforma_streaming PRIVATE streaming_core)

# ------------------------------------------------------------------
# Pruebas (ctest --test-dir build --output-on-failure)
# ------------------------------------------------------------------
enable_testing()

add_executable(tests_proyecto test_relevancia_recomendador.cpp test_motor.cpp)
target_link_libraries(tests_proyecto PRIVATE streaming_core)
add_test(NAME pruebas_del_proyecto COMMAND tests_proyecto)

# Arbol (Integrante 1 - Parte 2): pruebas unitarias y demo para la exposicion
add_executable(tests_arbol test_arbol.cpp)
target_link_libraries(tests_arbol PRIVATE streaming_core)
add_test(NAME pruebas_del_arbol COMMAND tests_arbol)

add_executable(demo_arbol demo_arbol.cpp)
target_link_libraries(demo_arbol PRIVATE streaming_core)

# Patrones A (Integrante 3 - Parte 2): Singleton + Factory
add_executable(test_patrones test_patrones.cpp)
target_link_libraries(test_patrones PRIVATE streaming_core)
add_test(NAME pruebas_de_patrones COMMAND test_patrones)

# ------------------------------------------------------------------
# Falta: Integrante 2 (carga paralela del CSV). Sumar sus .cpp a
# FUENTES_NUCLEO; los hilos ya estan enlazados arriba y el catalogo
# tiene CatalogoPeliculas::cargarDesdeVector() para recibir sus peliculas.
# ------------------------------------------------------------------
