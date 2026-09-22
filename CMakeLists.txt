cmake_minimum_required(VERSION 3.20)
project(proyecto_buscador_peliculas)

set(CMAKE_CXX_STANDARD 20)

# Ejecutable principal de la aplicacion (sin los tests)
add_executable(proyecto_buscador_peliculas
        main.cpp
        CSVReader.cpp
        Indexador.cpp
        Motor_de_busqueda.cpp
        Pelicula.cpp
        Recomendador.cpp
        Relevancia.cpp
        Tag.cpp
        Usuario.cpp
        Trie.cpp)

# Ejecutable separado solo para los tests
add_executable(tests_proyecto
        test_relevancia_recomendador.cpp
        test_motor.cpp
        CSVReader.cpp
        Indexador.cpp
        Motor_de_busqueda.cpp
        Pelicula.cpp
        Recomendador.cpp
        Relevancia.cpp
        Tag.cpp
        Usuario.cpp
        Trie.cpp)
