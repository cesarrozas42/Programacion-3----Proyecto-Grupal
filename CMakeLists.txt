cmake_minimum_required(VERSION 3.20)
project(plataforma_streaming LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante1_Arbol
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante2_Paralelismo
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante3_Singleton_Factory
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante4_Strategy_Observer
    ${CMAKE_CURRENT_SOURCE_DIR}/Integrante5_Interfaz
    ${CMAKE_CURRENT_SOURCE_DIR}/Parte1_Base)

set(FUENTES_NUCLEO
    Integrante1_Arbol/Trie.cpp
    Integrante1_Arbol/Indexador.cpp
    Integrante2_Paralelismo/ParallelEngine.cpp
    Integrante3_Singleton_Factory/CatalogoPeliculas.cpp
    Integrante3_Singleton_Factory/PeliculaFactory.cpp
    Integrante4_Strategy_Observer/EstrategiaBusqueda.cpp
    Parte1_Base/CSVReader.cpp
    Parte1_Base/Motor_de_busqueda.cpp
    Parte1_Base/Pelicula.cpp
    Parte1_Base/Recomendador.cpp
    Parte1_Base/Relevancia.cpp
    Parte1_Base/Tag.cpp
    Parte1_Base/Usuario.cpp)

add_library(streaming_core ${FUENTES_NUCLEO})

find_package(Threads REQUIRED)
target_link_libraries(streaming_core PUBLIC Threads::Threads)

if(MSVC)
    target_compile_options(streaming_core PRIVATE /W4)
else()
    target_compile_options(streaming_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(plataforma_streaming Integrante5_Interfaz/main.cpp Integrante5_Interfaz/InterfazCLI.cpp)
target_link_libraries(plataforma_streaming PRIVATE streaming_core)

enable_testing()

add_executable(tests_proyecto Parte1_Base/test_relevancia_recomendador.cpp Parte1_Base/test_motor.cpp)
target_link_libraries(tests_proyecto PRIVATE streaming_core)
add_test(NAME pruebas_del_proyecto COMMAND tests_proyecto)

add_executable(tests_arbol Integrante1_Arbol/test_arbol.cpp)
target_link_libraries(tests_arbol PRIVATE streaming_core)
add_test(NAME pruebas_del_arbol COMMAND tests_arbol)

add_executable(demo_arbol Integrante1_Arbol/demo_arbol.cpp)
target_link_libraries(demo_arbol PRIVATE streaming_core)

add_executable(test_patrones Integrante3_Singleton_Factory/test_patrones.cpp)
target_link_libraries(test_patrones PRIVATE streaming_core)
add_test(NAME pruebas_de_patrones COMMAND test_patrones)

add_executable(benchmark Integrante2_Paralelismo/benchmark.cpp)
target_link_libraries(benchmark PRIVATE streaming_core)
