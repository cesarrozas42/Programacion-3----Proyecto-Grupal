cmake_minimum_required(VERSION 3.20)
project(plataforma_streaming LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------
# Nucleo compartido por todos los ejecutables
# ------------------------------------------------------------------
set(FUENTES_NUCLEO
    CSVReader.cpp
    EstrategiaBusqueda.cpp # Integrante 4 (Parte 2): Strategy (arbol / secuencial / paralela)
    Indexador.cpp          # Integrante 1 (Parte 2): un arbol por tipo de tag
    Motor_de_busqueda.cpp
    Pelicula.cpp
    Recomendador.cpp
    Relevancia.cpp
    Tag.cpp
    Trie.cpp               # Integrante 1 (Parte 2): trie de palabras + trie de sufijos
    Usuario.cpp)

add_library(streaming_core ${FUENTES_NUCLEO})
target_include_directories(streaming_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# La estrategia paralela usa hilos (std::async / std::thread)
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

# ------------------------------------------------------------------
# Aqui se agregan los modulos de los demas integrantes cuando esten:
#  - Integrante 2 (carga paralela del CSV): sumar sus .cpp a FUENTES_NUCLEO
#    (los hilos ya estan enlazados arriba).
#  - Integrante 3 (Singleton / Factory): sumar sus .cpp a FUENTES_NUCLEO.
# ------------------------------------------------------------------
